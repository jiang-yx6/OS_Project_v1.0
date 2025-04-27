/*---------- process.h ----------*/
#ifndef PROCESS_H
#define PROCESS_H
#include<iostream>
#include<unordered_map>
#include<queue>
#include<deque>
#include<vector>
#include<string>
#include <thread>
#include <chrono>
#include <functional>
#include "../log.h"
#include "../state.h"
using namespace std;
enum class SchedulePolicy {
    PRIORITY,
    SJF, // ����ҵ����
    FCFS, //�����ȷ���
    RR //ʱ��Ƭ��ת
};
class PCB {
private:
    int pid;
    string name;
    ProcessState state;
    int priority; //���ȼ�
    int operaTime; //����ִ����ʱ��
    int remainTime;

public:
    PCB(int id, string pname, int pri, int opTime) {
        pid = id;
        name = pname;
        state = READY;
        priority = pri;
        operaTime = opTime;
        remainTime = opTime;
    }

    int getPid() const { return pid; }
    string getName() const { return name; }
    ProcessState getState() const { return state; }
    int getPriority() const { return priority; }
    int getRemainTime() const { return remainTime; }

    void setState(ProcessState newState) {
        ProcessState oldState = state;
        state = newState;
        Logger::getInstance()->logStateChange(name, pid, oldState, newState);
    }


    void decrementRemainTime() { if (remainTime > 0) remainTime--; } //����ʱ��Ƭ���ȵ�ʱ��

    bool isFinished() { return remainTime <= 0; }
};
class Timer {
private:
    std::thread timer_thread;
    atomic<bool> time_slice_expired;  // ʹ��ԭ�ӱ�������֤�̰߳�ȫ
    bool running;
    function<void()>callback;

public:
    Timer() : running(false), time_slice_expired(false) {}

    ~Timer() {
        stop();
    }

    void setCallBack(function<void()>cb) {
        callback = cb;
    }

    void start(int milliseconds) {
        stop();
        running = true;
        time_slice_expired = false;
        timer_thread = thread([this, milliseconds]() 
        {
            while (running) 
            {
                if (callback) {
                    std::cout << "[DEBUG] Timer callback triggered" << std::endl;
                    callback();
                }
                this_thread::sleep_for(chrono::milliseconds(milliseconds));
                if (running) {
                    time_slice_expired = true;
                    std::cout << "[DEBUG] Time slice expired flag set to true" << std::endl;
                }
            }
        });
    }

    void stop() {
        running = false;
        // ����֮��join�̣߳���������
        if (timer_thread.joinable()) {
            timer_thread.join();
        }
    }

    bool isTimeSliceExpired() {
        return time_slice_expired;
    }

    void resetTimeSliceFlag() {
        time_slice_expired = false;
    }
};
struct Comparator {
    bool operator()(PCB* p1, PCB* p2) {
        return p1->getPriority() > p2->getPriority(); //ֵԽС���ȼ�Խ��
    }
};
struct SJFComparator {
    bool operator()(PCB* p1, PCB* p2) {
        return p1->getRemainTime() > p2->getRemainTime();  // ʣ��ʱ��Խ�����ȼ�Խ��
    }
};
class ProcessManager {
private:
    unordered_map<int, PCB*> processMap;
    priority_queue<PCB*, vector<PCB*>, Comparator> readyQueue; //��������
    priority_queue<PCB*, vector<PCB*>, SJFComparator> sjfQueue; // ����ҵ���ȶ���
    queue<PCB*> blockQueue; //��������
    queue<PCB*> fcfsQueue; // FCFS ����
    deque<PCB*> rrQueue;    // ʱ��Ƭ��ת����
    PCB* runningProcess; //��ǰ�������н���
    int nextPid;
    int timeSlice;
    const int TIME_SLICE_MS = 1000; //����ʱ��Ƭ1��
    Timer* scheduleTimer;
    Logger* logger;
    SchedulePolicy policy;
public:
    ProcessManager(SchedulePolicy p = SchedulePolicy::PRIORITY) : policy(p) {
        nextPid = 0;
        runningProcess = nullptr;
        scheduleTimer = new Timer();
        timeSlice = 1;

        logger = Logger::getInstance();
        std::cout << "�����ԡ���ǰ���Ȳ�����: "
            << (policy == SchedulePolicy::SJF ? "SJF" :
                policy == SchedulePolicy::FCFS ? "FCFS" :
                policy == SchedulePolicy::RR ? "Round Robin" : "PRIORITY")
            << std::endl;

        //// ���ö�ʱ���Ļص�����
        //scheduleTimer->setCallBack([this]() { this->timeSliceExpired(); });
    }
    bool hasProcesses() const {
        return !processMap.empty() || runningProcess != nullptr;
    }

    Timer& getTimer() {
        return *scheduleTimer;
    }
    bool checkAndHandleTimeSlice();
    int createProcess(string name, int priority, int operaTime);
    void terminateProcess(int pid); //��ֹ�ն�
    void wakeupProcess(int pid); //�����ն�
    void blockProcess(int pid); //�����ն�

    void dispatcher();

    void timeSliceExpired();
    void addToReadyQueue(PCB* ptr);
    // �������������ProcessManager��
    ~ProcessManager() {
        // ��ֹͣ��ʱ��
        if (scheduleTimer) {
            scheduleTimer->stop();
            delete scheduleTimer;
            scheduleTimer = nullptr;
        }

        // Ȼ���������н���
        for (auto& pair : processMap) {
            delete pair.second;
        }
        processMap.clear();
    }
};
#endif