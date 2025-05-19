/*---------- process.h ----------*/
#ifndef PROCESS_H
#define PROCESS_H
#include<iostream>
#include<unordered_map>
#include<queue>
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
    int priority;
    int operaTime; //����ִ����ʱ��
    int remainTime;
    std::function<void()> task;

    /*int createdTime;
    int startedTime;
    int deleteTime;
    size_t memoryStart;
    size_t memorySize;
    vector<int> allocatedResources;
    vector<int> requestedResources;
    vector<int> openFiles;*/

public:
    PCB(int id, string pname, int pri, int opTime) {
        pid = id;
        name = pname;
        state = READY;
        priority = pri;
        operaTime = opTime;
        remainTime = opTime;
        
    }

    void registerFunc(std::function<void()> func) {
        task = std::move(func);
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
    Timer() : running(false),time_slice_expired(false) {}

    ~Timer() {
        stop();
    }

    void setCallBack(function<void()>cb) {
        callback = std::move(cb);
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
                    callback();
                    std::cout << "son Thread get outputMutex" << std::endl;
                }
                this_thread::sleep_for(chrono::milliseconds(milliseconds));

                time_slice_expired = true;
                running = false;
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
    queue<PCB*> fcfsQueue; // FCFS ����
    queue<PCB*> blockQueue; //��������
    deque<PCB*> rrQueue;    // ʱ��Ƭ��ת����
    PCB* runningProcess; //��ǰ�������н���
    int nextPid;
    int timeSlice;
    const int TIME_SLICE_MS = 1000; //����ʱ��Ƭ1��
    Timer* scheduleTimer;
    Logger* logger;
    SchedulePolicy policy;

    

    //��ӻ��������̼߳�ر��� 
    std::mutex outputMutex;
    std::atomic<bool> isMonitorRunning;
    std::thread timeSliceMonitorThread;
public:
    ProcessManager(SchedulePolicy p = SchedulePolicy::PRIORITY) : policy(p),isMonitorRunning(false) {
        nextPid = 0;
        runningProcess = nullptr;
        scheduleTimer = new Timer();
        timeSlice = 1;

        logger = Logger::getInstance();
    }

    bool hasProcesses() const {
        return !processMap.empty() || runningProcess != nullptr;
    }

    Timer& getTimer() {
        return *scheduleTimer;
    }
    bool checkAndHandleTimeSlice();
    int createProcess(string name, int priority, int operaTime, std::function<void()> func);
    void terminateProcess(int pid); //��ֹ�ն�
    void wakeupProcess(int pid); //�����ն�
    void blockProcess(int pid); //�����ն�

    void dispatcher();

    void timeSliceExpired();
    void addToReadyQueue(PCB* ptr);


    void timeSliceMonitorFunc();
    void startTimeSliceMonitor();
    void stopTimeSliceMonitor();


    std::mutex& getOutputMutex() {
        return outputMutex;
    }
    // �������������ProcessManager��
    ~ProcessManager() {
        stopTimeSliceMonitor();
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