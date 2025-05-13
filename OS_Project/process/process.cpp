#include"process.h"

int ProcessManager::createProcess(string name, int priority, int operaTime, std::function<void()> func) {
	int newpid = nextPid++;
	PCB* newProcess = new PCB(newpid, name, priority, operaTime);
	scheduleTimer->setCallBack(func);

	processMap[newpid] = newProcess;
	// ���ݵ��Ȳ��ԣ����½�����ӵ���Ӧ�Ķ���
	addToReadyQueue(newProcess);

	// ��������Ϣ��¼����־
	logger->logProcessCreation(name, newpid, priority, operaTime);

	//std::cout << "[DEBUG] ��ǰ processMap ��С: " << processMap.size() << std::endl;
	// ����ǵ�һ�����̣�ֱ�ӵ���ִ��
	if (processMap.size() == 1) {
		logger->logScheduling(name, newpid);
		dispatcher(); // ���ȸý���
	}

	// �����½��̵� PID
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
		runningProcess->decrementRemainTime(); // ����ʣ��ʱ��
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
				// ���������¼��뵽RR���е�ĩβ
				rrQueue.push_back(runningProcess);
			}
			else {
				// �������Ȳ��Խ��������¼��뵽��Ӧ�Ķ���
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
		// Ĭ�ϴ�����ѡ���׳��쳣����������ʽ
		break;
	}
}