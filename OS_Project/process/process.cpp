#include"process.h"

int ProcessManager::createProcess(string name, int priority, int operaTime, std::function<void()> func,string filename) {
	int newpid = nextPid++;
	FileResource* fileRes = nullptr;

	if(filename != ""){
		fileRes = fileResourceManager.getFileResource(filename,newpid);
	}
	
	PCB* newProcess = new PCB(newpid, name, priority, operaTime,fileRes);

	newProcess->registerFunc(func);

	processMap[newpid] = newProcess;
	// ���ݵ��Ȳ��ԣ����½�����ӵ���Ӧ�Ķ���
	addToReadyQueue(newpid);

	// ��������Ϣ��¼����־
	logger->logProcessCreation(name, newpid, priority, operaTime);

	//std::cout << "[DEBUG] ��ǰ processMap ��С: " << processMap.size() << std::endl;
	// ����ǵ�һ�����̣�ֱ�ӵ���ִ��
	if (processMap.size() <= thread_Pool.nums) {
		logger->logScheduling(name, newpid);
		dispatcher(); // ���ȸý���
	}

	// �����½��̵� PID
	return newpid;
}

void ProcessManager::terminateProcess(int pid) {
	if(processMap[pid]->fileRes)
		fileResourceManager.releaseFileAccess(pid, processMap[pid]->fileRes->filename); //�ͷ��ļ���Դ

	PCB* process = processMap[pid];
	process->setState(OVER);
	historyOverMap[process->getPid()] = new RabbishPCB(process->getPid(),process->getName(),process->getState(),  process->getPriority(), process->getRemainTime(), process->getCreateTime());
	processMap.erase(pid);
	delete process;
}

void ProcessManager::wakeupProcess(int pid) {}
void ProcessManager::blockProcess(int pid) {}
void ProcessManager::dispatcher() 
{	
	PCB* nextProcess = nullptr;

	switch (policy) {
	case SchedulePolicy::PRIORITY:
		if (!readyQueue.empty()) {
			nextProcess = readyQueue.top();
			//cout << "1 ready Queue Size is: "<< readyQueue.size() << endl;
			readyQueue.pop();
			//cout << "2 ready Queue Size is: " << readyQueue.size() << endl;

			//cout << "Dispacher result: "<< nextProcess->getName() << " [" << nextProcess->getPid() << "] is got from READY queue." << std::endl;
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
		//cout << "Process " << nextProcess->getName() << " [" << nextProcess->getPid() << "] is going to add to RUN queue." << std::endl;
		nextProcess->setState(RUNNING);
		
		if (nextProcess->fileRes) {
			bool acquired = fileResourceManager.tryRequestFileAccess(nextProcess->getPid(), nextProcess->fileRes->filename);

			//cout << " acquire��"<< acquired << endl;
			if (!acquired) {
				std::lock_guard<std::mutex> lock(blockQueueMutex);
				blockQueue.push(nextProcess->getPid());
				nextProcess->setState(BLOCKED);
				return; // ����ִ�к�������
			}
		}

		//�����ȡ���ļ�������������ж���
		thread_Pool.enqueue(nextProcess);
		logger->logScheduling(nextProcess->getName(), nextProcess->getPid());
	}
	else {
		nextProcess = nullptr;
	}
}
void ProcessManager::timeSliceExpired(int outpid) 
{
	PCB* outPCB = processMap[outpid];
	if (!outPCB) { cout << "PCB NULL 1" << endl; }
	outPCB->decrementRemainTime(); // ����ʣ��ʱ��
	//cout  << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] remaining time is " <<outPCB->getRemainTime() << std::endl;
	logger->logTimeSlice(outPCB->getName(),
		outPCB->getPid(),
		outPCB->getRemainTime());

	if (outPCB->isFinished()) {
		//cout << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] has completed." << endl;

		logger->logProcessCompletion(outPCB->getName(),
			outPCB->getPid());
		outPCB->getPTimer()->stop();
		terminateProcess(outPCB->getPid());

		//cout << "ProcessMap Size is: "<<processMap.size()<< endl;
		dispatcher();
	}
	else{
        //cout << "Process " << outPCB->getName() << " ["<< outPCB->getPid() << "] is not over and going to READY" << std::endl;
		outPCB->getPTimer()->stop();
		outPCB->setState(READY);

		if (policy == SchedulePolicy::RR) {
			// ���������¼��뵽RR���е�ĩβ
			rrQueue.push_back(outPCB);
		}
		else {
			// �������Ȳ��Խ��������¼��뵽��Ӧ�Ķ���
			addToReadyQueue(outpid);
		}
		outPCB = nullptr;
		dispatcher();
	}
}
bool ProcessManager::checkAndHandleTimeSlice() {
	{
		std::unique_lock<std::mutex> lock(thread_Pool.runningQueueMutex);
		int size = thread_Pool.comsumed_runningProcess.size();
		for (int i = 0; i < size; i++) {
			PCB* schedulePCB = thread_Pool.comsumed_runningProcess.front();

			thread_Pool.comsumed_runningProcess.pop(); //����ִ���������

			if (schedulePCB->getPTimer()->isTimeSliceExpired()) {
				schedulePCB->getPTimer()->resetTimeSliceFlag();
				timeSliceExpired(schedulePCB->getPid());
				return true;
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

	//cout << "Process " << processMap[pid]->getName() << " [" << processMap[pid]->getPid() << "] is added to READY queue." << std::endl;
}

void ProcessManager::timeSliceMonitorFunc() {
	while (isMonitorRunning.load()) {
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


void ProcessManager::blockMonitorThreadFunc() {
	while (isMonitorRunning.load()) {
		std::queue<int> tempQueue;

		// ���� blockQueue
		while (!blockQueue.empty()) {
			int pid = blockQueue.front();
			blockQueue.pop();

			PCB* pcb = processMap[pid];
			if (!pcb || pcb->getState() != BLOCKED) continue;

			bool acquired = fileResourceManager.tryRequestFileAccess(pcb->getPid(), pcb->fileRes->filename);

			if (acquired) {
				// �ɹ���ȡ��Դ �� ����������в�����
				cout<<"Process " << pcb->getName() << " [" << pcb->getPid() << "] has acquired file lock. Successfully" << std::endl;
				addToReadyQueue(pid);
				pcb->setState(READY);
				dispatcher(); // ��������
			}
			else {
				// ���޷���ȡ �� ������ʱ���У��Ժ����¼��� blockQueue
				tempQueue.push(pid);
			}
		}

		// ��δ�����ѵĽ������·Ż� blockQueue
		blockQueue = std::move(tempQueue);

		// ��Ϣһ��ʱ���ټ��
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void ProcessManager::startBlockMonitor() {
	if (!isBlockMonitorRunning.load()) {
		isBlockMonitorRunning.store(true);
		blockMonitorThread = std::thread(&ProcessManager::blockMonitorThreadFunc, this);
	}
}

void ProcessManager::stopBlockMonitor() {
	if (isBlockMonitorRunning.load()) {
		isBlockMonitorRunning.store(false);
		if (blockMonitorThread.joinable()) {
			blockMonitorThread.join();
		}
	}
}

