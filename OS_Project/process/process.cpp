#include"process.h"

int ProcessManager::createProcess(string name, int priority, int operaTime, std::function<void()> func) {
	int newpid = nextPid++;
	PCB* newProcess = new PCB(newpid, name, priority, operaTime);
	scheduleTimer->setCallBack(func);

	processMap[newpid] = newProcess;
	// 根据调度策略，将新进程添加到相应的队列
	addToReadyQueue(newProcess);

	// 将进程信息记录到日志
	logger->logProcessCreation(name, newpid, priority, operaTime);

	//std::cout << "[DEBUG] 当前 processMap 大小: " << processMap.size() << std::endl;
	// 如果是第一个进程，直接调度执行
	if (processMap.size() == 1) {
		logger->logScheduling(name, newpid);
		dispatcher(); // 调度该进程
	}

	// 返回新进程的 PID
	return newpid;
}
void ProcessManager::terminateProcess(int pid) {
	PCB* process = processMap[pid];
	process->setState(OVER);
	processMap.erase(pid);
	delete process;
}
void ProcessManager::wakeupProcess(int pid) {}
void ProcessManager::blockProcess(int pid) {}
void ProcessManager::dispatcher() 
{
	if (runningProcess) {
		scheduleTimer->stop();
	}

	PCB* nextProcess = nullptr;

	switch (policy) {
	case SchedulePolicy::PRIORITY:
		if (!readyQueue.empty()) {
			nextProcess = readyQueue.top();
			readyQueue.pop();
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
		runningProcess = nextProcess;
		runningProcess->setState(RUNNING);
		logger->logScheduling(runningProcess->getName(), runningProcess->getPid());
		scheduleTimer->start(TIME_SLICE_MS);
	}
	else {
		runningProcess = nullptr;
	}
}
void ProcessManager::timeSliceExpired() 
{
	if (runningProcess) 
	{
		runningProcess->decrementRemainTime(); // 减少剩余时间
		logger->logTimeSlice(runningProcess->getName(),
			runningProcess->getPid(),
			runningProcess->getRemainTime());

		if (runningProcess->isFinished()) {
			logger->logProcessCompletion(runningProcess->getName(),
				runningProcess->getPid());
			scheduleTimer->stop();
			terminateProcess(runningProcess->getPid());
			runningProcess = nullptr;
			dispatcher();
		}
		else if(policy == SchedulePolicy::FCFS)
		{}
		else{
			scheduleTimer->stop();
			runningProcess->setState(READY);

			if (policy == SchedulePolicy::RR) {
				// 将进程重新加入到RR队列的末尾
				rrQueue.push_back(runningProcess);
			}
			else {
				// 其他调度策略将进程重新加入到相应的队列
				addToReadyQueue(runningProcess);
			}
			runningProcess = nullptr;
			dispatcher();
		}
	}
}
bool ProcessManager::checkAndHandleTimeSlice() {
	{
		if (scheduleTimer->isTimeSliceExpired()) {
			scheduleTimer->resetTimeSliceFlag();
			timeSliceExpired();
			return true;
		}
		return false;
	}
}
void ProcessManager::addToReadyQueue(PCB* process) {
	switch (policy) {
	case SchedulePolicy::PRIORITY:
		readyQueue.push(process);
		break;
	case SchedulePolicy::SJF:
		sjfQueue.push(process);
		break;
	case SchedulePolicy::FCFS:
		fcfsQueue.push(process);
		break;
	case SchedulePolicy::RR:
		rrQueue.push_back(process);
		break;
	default:
		// 默认处理，可选择抛出异常或其他处理方式
		break;
	}
}