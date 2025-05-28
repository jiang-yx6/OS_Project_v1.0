#include"process.h"

Timer::Timer() : running(false), time_slice_expired(false), is_io_blocked_flag(false) {}
Timer::~Timer() { /* stop(); */ } // stop() 在新逻辑中可能无实际作用

void Timer::setCallBack(function<void()>cb) {
	callback = std::move(cb);
}
void Timer::setAssociatedPCB(PCB* pcb) {
	associatedPCB = pcb;
}

// !!! 关键修正：Timer::start() 的实现 !!!
// 这里能够访问 PCB 的成员 (pid, name, isIoIntensive)
// 因为在编译这个 .cpp 文件时，PCB 的完整定义已经通过 #include "process.h" 知道了
void Timer::start(int milliseconds) {
	// 确保状态干净
	time_slice_expired = false;
	is_io_blocked_flag = false;

	// 模拟 I/O 阻塞判断 (在任务执行之前)
	if (associatedPCB && associatedPCB->isIoIntensive) {
		if (rand() % 100 < 0) { // 30% 概率触发 I/O 阻塞
			/*Logger::getInstance()->log("进程 " + std::to_string(associatedPCB->pid) + "] " + associatedPCB->name + " 模拟 I/O 阻塞!");*/
			cout << "[PID " << associatedPCB->pid << "] " << associatedPCB->name << " simulated I/O blocking (Timer triggered)!" << endl;
			is_io_blocked_flag = true; // 设置I/O阻塞标志
			time_slice_expired = true; // 标记为“已处理”，因为要跳过时间片
			// running = false; // 在这里不再需要这个成员变量
			return; // 立即返回，表示进程因I/O阻塞而停止
		}
	}

	// 执行进程任务 (回调)
	if (callback) {
		callback(); // 进程的实际工作在这里执行一次
		//std::cout << "son Thread get outputMutex" << std::endl; // 移除或在callback内部处理
	}

	// 模拟时间流逝
	std::this_thread::sleep_for(chrono::milliseconds(milliseconds));

	// 标记时间片正常结束（如果之前没有因I/O阻塞而返回）
	time_slice_expired = true;
}

void Timer::stop() {
	// 在新逻辑中，Timer 不再是持续运行的线程，这个 stop 方法可以空着。
	// 如果你依然想用 running 标志来中断 sleep，则需要一个单独的线程 for Timer::start
	// 但当前更简单的模型是 Timer::start 阻塞一次然后返回。
}

bool Timer::isTimeSliceExpired() {
	return time_slice_expired.load(); // 使用 .load() 访问原子变量
}
bool Timer::isIoBlocked() {
	return is_io_blocked_flag.load(); // 使用 .load() 访问原子变量
}
void Timer::resetTimeSliceFlag() {

	time_slice_expired.store(false);
	is_io_blocked_flag.store(false); // 同时重置I/O阻塞标志
}

PCB::PCB(int id, string pname, int pri, int opTime) {
	pid = id;
	name = pname;
	state = READY;
	priority = pri;
	operaTime = opTime;
	remainTime = opTime;
	scheduleTimer = new Timer(); // 先创建 Timer
	scheduleTimer->setAssociatedPCB(this); // 然后设置关联的 PCB
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

	// !!! 关键改动：根据命令名称设置 isIoIntensive 标志 !!!
	if (name == "ls" || name == "cat" || name == "echo" || name == "login" || name == "mkdir" || name == "rmdir" || name == "mkfile" || name == "rmfile" || name == "permission" || name == "vim") {
		newProcess->isIoIntensive = true;
	}
	else {
		newProcess->isIoIntensive = false;
	}


	processMap[newpid] = newProcess;
	// 根据调度策略，将新进程添加到相应的队列
	addToReadyQueue(newpid);

	// 将进程信息记录到日志
	logger->logProcessCreation(name, newpid, priority, operaTime);

	//std::cout << "[DEBUG] 当前 processMap 大小: " << processMap.size() << std::endl;
	// 如果是第一个进程，直接调度执行
	if (processMap.size() <= 2) {
		logger->logScheduling(name, newpid);
		dispatcher(); // 调度该进程
	}

	// 返回新进程的 PID
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
// 从blockQueue中找到并移除PCB，然后添加到就绪队列
// 注意：std::queue不支持直接遍历和移除中间元素，这里需要改进或简化
// 最简单的模拟：模拟I/O完成时直接从blockedQueue中移除 front()
// 或者，如果使用std::list<PCB*> blockedQueue, 则可以遍历查找并erase
	PCB* pcb = processMap[pid]; // 假设PCBs都在processMap中，所以可以通过pid找到
	if (pcb && pcb->getState() == BLOCKED) {
		pcb->setState(READY);
		addToReadyQueue(pid);
		// logger->logProcessUnblocked(pcb->getName(), pcb->getPid()); // 如果有对应日志方法
		cout << "Process " << pcb->getName() << " [" << pcb->getPid() << "] is WAKEUP (from BLOCKED to READY)." << endl;
		// 唤醒线程池的消费者线程
		thread_Pool.condition.notify_all();
	}
}
void ProcessManager::blockProcess(int pid) {
	PCB* pcb = processMap[pid];
	if (pcb) {
		pcb->setState(BLOCKED);
		blockQueue.push(pcb);
		// logger->logProcessBlocked(pcb->getName(), pcb->getPid()); // 如果有对应日志方法
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
//	// --- 新增 I/O 阻塞处理 ---
//	if (outPCB->getPTimer()->isIoBlocked()) {
//		cout << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] is BLOCKED (I/O) and put into BLOCK queue." << std::endl;
//		outPCB->getPTimer()->resetTimeSliceFlag(); // 重置Timer标志
//		blockProcess(outpid); // 调用新的阻塞函数
//		//dispatcher(); // 立即调度下一个进程
//		return; // 处理完毕，返回
//	}
//
//
//	outPCB->decrementRemainTime(); // 减少剩余时间
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
//			// 将进程重新加入到RR队列的末尾
//			rrQueue.push_back(outPCB);
//		}
//		else {
//			// 其他调度策略将进程重新加入到相应的队列
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

    // !!! 核心修正：首先检查是否因I/O阻塞 !!!
    if (outPCB->getPTimer()->isIoBlocked()) {
        cout << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] is BLOCKED (I/O) and put into BLOCK queue." << std::endl;
        // outPCB->getPTimer()->stop(); // 如果Timer.stop()只是设置running=false，可移除
        outPCB->setState(BLOCKED); // 设置状态
        blockProcess(outpid); // 将进程放入阻塞队列（这个函数内部会push到blockQueue）
        // outPCB = nullptr; // 这个赋值没意义，outPCB是局部变量
    } else { // 否则，按正常时间片过期或完成处理
        outPCB->decrementRemainTime(); // 减少剩余时间
        cout  << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] remaining time is " <<outPCB->getRemainTime() << std::endl;
        logger->logTimeSlice(outPCB->getName(),
            outPCB->getPid(),
            outPCB->getRemainTime());

        if (outPCB->isFinished()) {
            cout << "Process " << outPCB->getName() << " [" << outPCB->getPid() << "] has completed." << endl;
            logger->logProcessCompletion(outPCB->getName(),
                outPCB->getPid());
            // outPCB->getPTimer()->stop(); // 如果Timer.stop()只是设置running=false，可移除
            terminateProcess(outPCB->getPid());
            cout << "ProcessMap Size is: "<<processMap.size()<< endl;
        } else {
            cout << "Process " << outPCB->getName() << " ["<< outPCB->getPid() << "] is not over and going to READY" << std::endl;
            // outPCB->getPTimer()->stop(); // 如果Timer.stop()只是设置running=false，可移除
            outPCB->setState(READY);

            if (policy == SchedulePolicy::RR) {
                rrQueue.push_back(outPCB);
            } else {
                addToReadyQueue(outpid);
            }
			// !!! 核心改动：在所有处理逻辑完成后，才重置Timer标志 !!!
			outPCB->getPTimer()->resetTimeSliceFlag();
        }
    }
    
    dispatcher(); // 无论进程是阻塞、完成还是回到就绪，都尝试调度下一个
}


bool ProcessManager::checkAndHandleTimeSlice() {
	{
		//std::unique_lock<std::mutex> lock(thread_Pool.runningQueueMutex);
		int size = thread_Pool.comsumed_runningProcess.size();
		//cout << "Checking Thread get runningQueueMutex And the size is " <<size<< std::endl;
		for (int i = 0; i < size; i++) {
			PCB* schedulePCB = thread_Pool.comsumed_runningProcess.front();

			thread_Pool.comsumed_runningProcess.pop(); //弹出执行完的任务。

			//if (schedulePCB->getPTimer()->isTimeSliceExpired()) {
			//	schedulePCB->getPTimer()->resetTimeSliceFlag();
			//	cout << "Process " << schedulePCB->getName() << " [" << schedulePCB->getPid() << "] has time slice expired." << std::endl;
			//	timeSliceExpired(schedulePCB->getPid());
			//	return true;
			//}
			if (schedulePCB->getPTimer()->isTimeSliceExpired()) {
				cout << "Process " << schedulePCB->getName() << " [" << schedulePCB->getPid() << "] has time slice expired (Monitor detected)." << std::endl;
				timeSliceExpired(schedulePCB->getPid()); // 交给 timeSliceExpired 处理并重置标志
				// return true; // 如果只处理一个，这里返回
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
		// 默认处理，可选择抛出异常或其他处理方式
		break;
	}

	cout << "Process " << processMap[pid]->getName() << " [" << processMap[pid]->getPid() << "] is added to READY queue." << std::endl;
}

void ProcessManager::timeSliceMonitorFunc() {
	while (isMonitorRunning.load()) {

		std::unique_lock<std::mutex> lock(pmMutex); // 确保在检查和修改队列时线程安全

		// 2. !!! 新增：处理阻塞队列的进程 (模拟 I/O 完成) !!!
		// 这里我们使用std::queue，所以只能从front()检查和pop()
		// 真实情况可能需要一个std::list或更复杂的管理来按条件移除
		// 简化处理：假设第一个阻塞的进程在一定时间后完成
		if (!blockQueue.empty()) {
			PCB* blockedPCB = blockQueue.front();
			// 模拟 I/O 完成条件：随机或基于时间
			// 我们可以给 PCB 一个 I/O 开始时间，然后等待一段时间
			// 这里用一个简单的随机数模拟
			if (rand() % 100 < 5) { // 5% 概率I/O完成
				cout << "--- [I/O Monitor] I/O for PID " << blockedPCB->getPid() << " (" << blockedPCB->getName() << ") completed. ---" << endl;
				blockQueue.pop(); // 从阻塞队列移除
				wakeupProcess(blockedPCB->getPid()); // 唤醒进程（会加入就绪队列）
			}
		}

		// 如果没有进程需要处理，或者已经处理完，可以释放锁并休眠
		lock.unlock(); // 释放锁，让其他线程有机会获取

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

