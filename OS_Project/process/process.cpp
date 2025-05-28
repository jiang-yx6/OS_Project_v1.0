#include"process.h"

Timer::Timer() : running(false), time_slice_expired(false), is_io_blocked_flag(false) {}
Timer::~Timer() { /* stop(); */ } // stop() �����߼��п�����ʵ������

void Timer::setCallBack(function<void()>cb) {
	callback = std::move(cb);
}
void Timer::setAssociatedPCB(PCB* pcb) {
	associatedPCB = pcb;
}

// !!! �ؼ�������Timer::start() ��ʵ�� !!!
// �����ܹ����� PCB �ĳ�Ա (pid, name, isIoIntensive)
// ��Ϊ�ڱ������ .cpp �ļ�ʱ��PCB �����������Ѿ�ͨ�� #include "process.h" ֪����
void Timer::start(int milliseconds) {
	// ȷ��״̬�ɾ�
	time_slice_expired = false;
	is_io_blocked_flag = false;

	// ģ�� I/O �����ж� (������ִ��֮ǰ)
	if (associatedPCB && associatedPCB->isIoIntensive) {
		if (rand() % 100 < 0) { // 30% ���ʴ��� I/O ����
			/*Logger::getInstance()->log("���� " + std::to_string(associatedPCB->pid) + "] " + associatedPCB->name + " ģ�� I/O ����!");*/
			cout << "[PID " << associatedPCB->pid << "] " << associatedPCB->name << " simulated I/O blocking (Timer triggered)!" << endl;
			is_io_blocked_flag = true; // ����I/O������־
			time_slice_expired = true; // ���Ϊ���Ѵ�������ΪҪ����ʱ��Ƭ
			// running = false; // �����ﲻ����Ҫ�����Ա����
			return; // �������أ���ʾ������I/O������ֹͣ
		}
	}

	// ִ�н������� (�ص�)
	if (callback) {
		callback(); // ���̵�ʵ�ʹ���������ִ��һ��
		//std::cout << "son Thread get outputMutex" << std::endl; // �Ƴ�����callback�ڲ�����
	}

	// ģ��ʱ������
	std::this_thread::sleep_for(chrono::milliseconds(milliseconds));

	// ���ʱ��Ƭ�������������֮ǰû����I/O���������أ�
	time_slice_expired = true;
}

void Timer::stop() {
	// �����߼��У�Timer �����ǳ������е��̣߳���� stop �������Կ��š�
	// �������Ȼ���� running ��־���ж� sleep������Ҫһ���������߳� for Timer::start
	// ����ǰ���򵥵�ģ���� Timer::start ����һ��Ȼ�󷵻ء�
}

bool Timer::isTimeSliceExpired() {
	return time_slice_expired.load(); // ʹ�� .load() ����ԭ�ӱ���
}
bool Timer::isIoBlocked() {
	return is_io_blocked_flag.load(); // ʹ�� .load() ����ԭ�ӱ���
}
void Timer::resetTimeSliceFlag() {

	time_slice_expired.store(false);
	is_io_blocked_flag.store(false); // ͬʱ����I/O������־
}

PCB::PCB(int id, string pname, int pri, int opTime) {
	pid = id;
	name = pname;
	state = READY;
	priority = pri;
	operaTime = opTime;
	remainTime = opTime;
	scheduleTimer = new Timer(); // �ȴ��� Timer
	scheduleTimer->setAssociatedPCB(this); // Ȼ�����ù����� PCB
}

void PCB::registerFunc(std::function<void()> func) {
	task = std::move(func);
	scheduleTimer->setCallBack(task);
}
Timer* PCB::getPTimer() { return scheduleTimer; }
function<void()> PCB::getTask() { return task; }
int PCB::getPid() const { return pid; }
string PCB::getName() const { return name; }
ProcessState PCB::getState() const { return state; }
int PCB::getPriority() const { return priority; }
int PCB::getRemainTime() const { return remainTime; }

void PCB::setState(ProcessState newState) {
	ProcessState oldState = state;
	state = newState;
	Logger::getInstance()->logStateChange(name, pid, oldState, newState);
}

void PCB::decrementRemainTime() { if (remainTime > 0) remainTime--; }
bool PCB::isFinished() { return remainTime <= 0; }

int ProcessManager::createProcess(string name, int priority, int operaTime, std::function<void()> func) {
	int newpid = nextPid++;
	PCB* newProcess = new PCB(newpid, name, priority, operaTime);
	newProcess->registerFunc(func);

	// !!! �ؼ��Ķ������������������� isIoIntensive ��־ !!!
	if (name == "ls" || name == "cat" || name == "echo" || name == "login" || name == "mkdir" || name == "rmdir" || name == "mkfile" || name == "rmfile" || name == "permission" || name == "vim") {
		newProcess->isIoIntensive = true;
	}
	else {
		newProcess->isIoIntensive = false;
	}


	processMap[newpid] = newProcess;
	// ���ݵ��Ȳ��ԣ����½�����ӵ���Ӧ�Ķ���
	addToReadyQueue(newpid);

	// ��������Ϣ��¼����־
	logger->logProcessCreation(name, newpid, priority, operaTime);

	//std::cout << "[DEBUG] ��ǰ processMap ��С: " << processMap.size() << std::endl;
	// ����ǵ�һ�����̣�ֱ�ӵ���ִ��
	if (processMap.size() <= 2) {
		logger->logScheduling(name, newpid);
		dispatcher(); // ���ȸý���
	}

	// �����½��̵� PID
	return newpid;
}

void ProcessManager::terminateProcess(int pid) {
	PCB* process = processMap[pid];
	process->setState(OVER);
	historyOverMap[process->getPid()] = process->getName();
	processMap.erase(pid);
	delete process;
}

void ProcessManager::wakeupProcess(int pid) {
// ��blockQueue���ҵ����Ƴ�PCB��Ȼ����ӵ���������
// ע�⣺std::queue��֧��ֱ�ӱ������Ƴ��м�Ԫ�أ�������Ҫ�Ľ����
// ��򵥵�ģ�⣺ģ��I/O���ʱֱ�Ӵ�blockedQueue���Ƴ� front()
// ���ߣ����ʹ��std::list<PCB*> blockedQueue, ����Ա������Ҳ�erase
	PCB* pcb = processMap[pid]; // ����PCBs����processMap�У����Կ���ͨ��pid�ҵ�
	if (pcb && pcb->getState() == BLOCKED) {
		pcb->setState(READY);
		addToReadyQueue(pid);
		// logger->logProcessUnblocked(pcb->getName(), pcb->getPid()); // ����ж�Ӧ��־����
		cout << "Process " << pcb->getName() << " [" << pcb->getPid() << "] is WAKEUP (from BLOCKED to READY)." << endl;
		// �����̳߳ص��������߳�
		thread_Pool.condition.notify_all();
	}
}
void ProcessManager::blockProcess(int pid) {
	PCB* pcb = processMap[pid];
	if (pcb) {
		pcb->setState(BLOCKED);
		blockQueue.push(pcb);
		// logger->logProcessBlocked(pcb->getName(), pcb->getPid()); // ����ж�Ӧ��־����
		cout << "Process " << pcb->getName() << " [" << pcb->getPid() << "] is BLOCKED." << endl;
	}
}
void ProcessManager::dispatcher() 
{	
	PCB* nextProcess = nullptr;

	switch (policy) {
	case SchedulePolicy::PRIORITY:
		if (!readyQueue.empty()) {
			nextProcess = readyQueue.top();
			cout << "1 ready Queue Size is: "<< readyQueue.size() << endl;
			readyQueue.pop();
			cout << "2 ready Queue Size is: " << readyQueue.size() << endl;

			cout << "Dispacher result: "<< nextProcess->getName() << " [" << nextProcess->getPid() << "] is got from READY queue." << std::endl;
		}
		break;
	case SchedulePolicy::SJF:
		if (!sjfQueue.empty()) {
			nextProcess = sjfQueue.top();
			sjfQueue.pop();
		}
		break;
	case SchedulePolicy::FCFS:
		if (!fcfsQueue.empty()) {
			nextProcess = fcfsQueue.front();
			fcfsQueue.pop();
		}
		break;
	case SchedulePolicy::RR:
		if (!rrQueue.empty()) {
			nextProcess = rrQueue.front();
			rrQueue.pop_front();
		}
		break;
	default:
		break;
	}
	
	if (nextProcess) {
		cout << "Process " << nextProcess->getName() << " [" << nextProcess->getPid() << "] is going to add to RUN queue." << std::endl;
		thread_Pool.enqueue(nextProcess);
		nextProcess->setState(RUNNING);
		logger->logScheduling(nextProcess->getName(), nextProcess->getPid());
	}
	else {
		nextProcess = nullptr;
	}
}
//void ProcessManager::timeSliceExpired(int outpid) 
//{
//	PCB* outPCB = processMap[outpid];
//	if (!outPCB) { cout << "PCB NULL 1" << endl; }
//
//	// --- ���� I/O �������� ---
//	if (outPCB->getPTimer()->isIoBlocked()) {
//		cout << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] is BLOCKED (I/O) and put into BLOCK queue." << std::endl;
//		outPCB->getPTimer()->resetTimeSliceFlag(); // ����Timer��־
//		blockProcess(outpid); // �����µ���������
//		//dispatcher(); // ����������һ������
//		return; // ������ϣ�����
//	}
//
//
//	outPCB->decrementRemainTime(); // ����ʣ��ʱ��
//	cout  << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] remaining time is " <<outPCB->getRemainTime() << std::endl;
//	logger->logTimeSlice(outPCB->getName(),
//		outPCB->getPid(),
//		outPCB->getRemainTime());
//
//	if (outPCB->isFinished()) {
//		cout << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] has completed." << endl;
//
//		logger->logProcessCompletion(outPCB->getName(),
//			outPCB->getPid());
//		/*outPCB->getPTimer()->stop();*/
//		terminateProcess(outPCB->getPid());
//
//		cout << "ProcessMap Size is: "<<processMap.size()<< endl;
//		dispatcher();
//	}
//	else{
//        cout << "Process " << outPCB->getName() << " ["<< outPCB->getPid() << "] is not over and going to READY" << std::endl;
//		/*outPCB->getPTimer()->stop();*/
//		outPCB->setState(READY);
//
//		if (policy == SchedulePolicy::RR) {
//			// ���������¼��뵽RR���е�ĩβ
//			rrQueue.push_back(outPCB);
//		}
//		else {
//			// �������Ȳ��Խ��������¼��뵽��Ӧ�Ķ���
//			addToReadyQueue(outpid);
//		}
//		/*outPCB = nullptr;*/
//		dispatcher();
//	}
//
//}
void ProcessManager::timeSliceExpired(int outpid) {
    PCB* outPCB = processMap[outpid];
    if (!outPCB) {
        cout << "Error: PCB NULL for pid " << outpid << endl;
        return;
    }

    // !!! �������������ȼ���Ƿ���I/O���� !!!
    if (outPCB->getPTimer()->isIoBlocked()) {
        cout << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] is BLOCKED (I/O) and put into BLOCK queue." << std::endl;
        // outPCB->getPTimer()->stop(); // ���Timer.stop()ֻ������running=false�����Ƴ�
        outPCB->setState(BLOCKED); // ����״̬
        blockProcess(outpid); // �����̷����������У���������ڲ���push��blockQueue��
        // outPCB = nullptr; // �����ֵû���壬outPCB�Ǿֲ�����
    } else { // ���򣬰�����ʱ��Ƭ���ڻ���ɴ���
        outPCB->decrementRemainTime(); // ����ʣ��ʱ��
        cout  << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] remaining time is " <<outPCB->getRemainTime() << std::endl;
        logger->logTimeSlice(outPCB->getName(),
            outPCB->getPid(),
            outPCB->getRemainTime());

        if (outPCB->isFinished()) {
            cout << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] has completed." << endl;
            logger->logProcessCompletion(outPCB->getName(),
                outPCB->getPid());
            // outPCB->getPTimer()->stop(); // ���Timer.stop()ֻ������running=false�����Ƴ�
            terminateProcess(outPCB->getPid());
            cout << "ProcessMap Size is: "<<processMap.size()<< endl;
        } else {
            cout << "Process " << outPCB->getName() << " ["<< outPCB->getPid() << "] is not over and going to READY" << std::endl;
            // outPCB->getPTimer()->stop(); // ���Timer.stop()ֻ������running=false�����Ƴ�
            outPCB->setState(READY);

            if (policy == SchedulePolicy::RR) {
                rrQueue.push_back(outPCB);
            } else {
                addToReadyQueue(outpid);
            }
			// !!! ���ĸĶ��������д����߼���ɺ󣬲�����Timer��־ !!!
			outPCB->getPTimer()->resetTimeSliceFlag();
        }
    }
    
    dispatcher(); // ���۽�������������ɻ��ǻص������������Ե�����һ��
}


bool ProcessManager::checkAndHandleTimeSlice() {
	{
		//std::unique_lock<std::mutex> lock(thread_Pool.runningQueueMutex);
		int size = thread_Pool.comsumed_runningProcess.size();
		//cout << "Checking Thread get runningQueueMutex And the size is " <<size<< std::endl;
		for (int i = 0; i < size; i++) {
			PCB* schedulePCB = thread_Pool.comsumed_runningProcess.front();

			thread_Pool.comsumed_runningProcess.pop(); //����ִ���������

			//if (schedulePCB->getPTimer()->isTimeSliceExpired()) {
			//	schedulePCB->getPTimer()->resetTimeSliceFlag();
			//	cout << "Process " << schedulePCB->getName() << " [" << schedulePCB->getPid() << "] has time slice expired." << std::endl;
			//	timeSliceExpired(schedulePCB->getPid());
			//	return true;
			//}
			if (schedulePCB->getPTimer()->isTimeSliceExpired()) {
				cout << "Process " << schedulePCB->getName() << " [" << schedulePCB->getPid() << "] has time slice expired (Monitor detected)." << std::endl;
				timeSliceExpired(schedulePCB->getPid()); // ���� timeSliceExpired �������ñ�־
				// return true; // ���ֻ����һ�������ﷵ��
			}
			else {
				thread_Pool.comsumed_runningProcess.push(schedulePCB);
			}
		}
		
		return false;
	}
}
void ProcessManager::addToReadyQueue(int pid) {
	switch (policy) {
	case SchedulePolicy::PRIORITY:
		readyQueue.push(processMap[pid]);
		break;
	case SchedulePolicy::SJF:
		sjfQueue.push(processMap[pid]);
		break;
	case SchedulePolicy::FCFS:
		fcfsQueue.push(processMap[pid]);
		break;
	case SchedulePolicy::RR:
		rrQueue.push_back(processMap[pid]);
		break;
	default:
		// Ĭ�ϴ�����ѡ���׳��쳣����������ʽ
		break;
	}

	cout << "Process " << processMap[pid]->getName() << " [" << processMap[pid]->getPid() << "] is added to READY queue." << std::endl;
}

void ProcessManager::timeSliceMonitorFunc() {
	while (isMonitorRunning.load()) {

		std::unique_lock<std::mutex> lock(pmMutex); // ȷ���ڼ����޸Ķ���ʱ�̰߳�ȫ

		// 2. !!! �����������������еĽ��� (ģ�� I/O ���) !!!
		// ��������ʹ��std::queue������ֻ�ܴ�front()����pop()
		// ��ʵ���������Ҫһ��std::list������ӵĹ������������Ƴ�
		// �򻯴��������һ�������Ľ�����һ��ʱ������
		if (!blockQueue.empty()) {
			PCB* blockedPCB = blockQueue.front();
			// ģ�� I/O �����������������ʱ��
			// ���ǿ��Ը� PCB һ�� I/O ��ʼʱ�䣬Ȼ��ȴ�һ��ʱ��
			// ������һ���򵥵������ģ��
			if (rand() % 100 < 5) { // 5% ����I/O���
				cout << "--- [I/O Monitor] I/O for PID " << blockedPCB->getPid() << " (" << blockedPCB->getName() << ") completed. ---" << endl;
				blockQueue.pop(); // �����������Ƴ�
				wakeupProcess(blockedPCB->getPid()); // ���ѽ��̣������������У�
			}
		}

		// ���û�н�����Ҫ���������Ѿ������꣬�����ͷ���������
		lock.unlock(); // �ͷ������������߳��л����ȡ

		if (hasProcesses()) {
			bool timeSliceExpired = checkAndHandleTimeSlice();
			if (!timeSliceExpired) {
                std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			}
		}
		else {
			std::this_thread::sleep_for(std::chrono::milliseconds(200));
		}
	}
}

void ProcessManager::startTimeSliceMonitor() {
	if (!isMonitorRunning.load()) {
		isMonitorRunning.store(true);
		timeSliceMonitorThread = std::thread(&ProcessManager::timeSliceMonitorFunc, this);
	}
}

void ProcessManager::stopTimeSliceMonitor() {
	if (isMonitorRunning.load()) {
		isMonitorRunning.store(false);
		if (timeSliceMonitorThread.joinable()) {
			timeSliceMonitorThread.join();
		}
	}
}

