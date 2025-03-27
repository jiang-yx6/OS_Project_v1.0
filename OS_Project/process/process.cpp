#include"process.h"

int ProcessManager::createProcess(string name, int priority, int operaTime) {
	int newpid = nextPid++;
	PCB* newProcess = new PCB(newpid, name, priority, operaTime);
	processMap[newpid] = newProcess;
	readyQueue.push(newProcess);
	logger->logProcessCreation(name, newpid, priority, operaTime);

	if (processMap.size() == 1) { //第一个进程直接调度到运行状态
		logger->logScheduling(name, newpid);
		dispatcher();
	}
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
void ProcessManager::dispatcher() {
	if (runningProcess) {
		scheduleTimer->stop();
	}
	if (readyQueue.empty()) {
		runningProcess = nullptr;
		return;
	}

	runningProcess = readyQueue.top();
	readyQueue.pop();
	runningProcess->setState(RUNNING);
	logger->logScheduling(runningProcess->getName(), runningProcess->getPid());

	scheduleTimer->start(TIME_SLICE_MS);

	
}

void ProcessManager::timeSliceExpired() {
	if (runningProcess) {
		runningProcess->decrementRemainTime(); //减少剩余时间

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
		else { //放回就绪队列

			scheduleTimer->stop();

			runningProcess->setState(READY);
			
			readyQueue.push(runningProcess); //重新放回优先级队列

			runningProcess = nullptr;

			dispatcher();//调度算法来实现进程调度
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
