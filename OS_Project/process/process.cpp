#include"process.h"

int ProcessManager::createProcess(string name, int priority, int operaTime) {
	int newpid = nextPid++;
	PCB* newProcess = new PCB(newpid, name, priority, operaTime);
	processMap[newpid] = newProcess;
	readyQueue.push(newProcess);

	if (processMap.size() == 1) { //第一个进程直接调度到运行状态
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

	scheduleTimer->start(TIME_SLICE_MS);

	cout << "Process " << runningProcess->getName()
		<< " (PID: " << runningProcess->getPid()
		<< ") started running." << endl;
	scheduleTimer->start(TIME_SLICE_MS);

}
void ProcessManager::timeSliceExpired() {
	if (runningProcess) {
		runningProcess->decrementRemainTime(); //减少剩余时间

		if (runningProcess->isFinished()) {
			cout << "Process " << runningProcess->getName()
				<< "(PID: " << runningProcess->getPid()
				<< ") completed" << endl;
			scheduleTimer->stop();

			terminateProcess(runningProcess->getPid());
			runningProcess = nullptr;
			dispatcher();
		}
		else { //放回就绪队列
			scheduleTimer->stop();

			runningProcess->setState(READY);

			readyQueue.push(runningProcess); //重新放回优先级队列

			cout << "Process " << runningProcess->getName()
				<< " (PID: " << runningProcess->getPid()
				<< ") time slice expired." << endl;
			runningProcess = nullptr;

			dispatcher();//调度算法来实现进程调度
		}
	}
}

