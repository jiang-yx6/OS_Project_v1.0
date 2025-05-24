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

const int TIME_SLICE_MS = 1000;

enum class SchedulePolicy {
    PRIORITY,
    SJF, // ����ҵ����
    FCFS, //�����ȷ���
    RR //ʱ��Ƭ��ת
};
class Timer {
public:
    atomic<bool> time_slice_expired;  // ʹ��ԭ�ӱ�������֤�̰߳�ȫ
    bool running;
    function<void()>callback;

public:
    Timer() : running(false), time_slice_expired(false) {}

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

    }

    void stop() {
        running = false;
        // ����֮��join�̣߳���������
    }

    bool isTimeSliceExpired() {
        return time_slice_expired;
    }

    void resetTimeSliceFlag() {
        time_slice_expired = false;
    }
};


class PCB {
public:
    int pid;
    string name;
    ProcessState state;
    int priority;
    int operaTime; //����ִ����ʱ��
    int remainTime;
    std::function<void()> task;
    Timer* scheduleTimer;

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
        scheduleTimer = new Timer();
    }

    void registerFunc(std::function<void()> func) {
        task = std::move(func);
        scheduleTimer->setCallBack(task);
    }
    Timer* getPTimer(){return scheduleTimer;}
    function<void()> getTask() { return task; }
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
class ThreadPool {
public:
    int nums = 2;
    ThreadPool() {
        for (int i = 0; i < nums; i++) {
            threads.emplace_back([this] {
                while (1) {
                    std::unique_lock<std::mutex> lock(runningQueueMutex);
                    condition.wait(lock, [this] {
                        return !noComsumed_runningProcess.empty();
                        });
                    cout<< "Consumer Thread get runningQueueMutex" << std::endl;
                    cout << "process remain time: " << noComsumed_runningProcess.front()->getRemainTime() << std::endl;
                    cout << "1 noComsumed Queue Size: " << noComsumed_runningProcess.size() << std::endl;

                    if (noComsumed_runningProcess.empty()) return;
                    Timer* timer(noComsumed_runningProcess.front()->getPTimer());
                    comsumed_runningProcess.push(noComsumed_runningProcess.front());
                    cout << "ComsumedQueue Queue Size: " << comsumed_runningProcess.size() << std::endl;
                    noComsumed_runningProcess.pop();
                    cout<<"2 noComsumed Queue Size: "<< noComsumed_runningProcess.size() << std::endl;
                    lock.unlock();
                    timer->start(TIME_SLICE_MS);
                }
                });
        }
    }

    void enqueue(PCB* readyToRunning) {
        //std::function<void()> task = readyToRunning->getTask();
        {
            cout << "Producer Thread trying to get  outputMutex" << std::endl;
            std::unique_lock<std::mutex> lock(runningQueueMutex);
            cout << "Producer Thread get runningQueueMutex" << std::endl;
            noComsumed_runningProcess.emplace(readyToRunning);
            curRunningLen++;
        }
        condition.notify_all();
    }

    vector<thread> threads;
    queue<PCB*> noComsumed_runningProcess;
    queue<PCB*> comsumed_runningProcess;
    std::atomic<int> curRunningLen = 0;
    std::mutex runningQueueMutex;
    std::condition_variable condition;

    ~ThreadPool() {
        for (auto& thread : threads) {
            thread.join();
        }
    }

    //friend class ProcessManager;
    mutex& getMutex() {
        return runningQueueMutex;
    }

};

class ProcessManager {
public:
    unordered_map<int, PCB*> processMap;
    unordered_map<int, string> historyOverMap;

    priority_queue<PCB*, vector<PCB*>, Comparator> readyQueue; //��������
    priority_queue<PCB*, vector<PCB*>, SJFComparator> sjfQueue; // ����ҵ���ȶ���
    queue<PCB*> fcfsQueue; // FCFS ����
    queue<PCB*> blockQueue; //��������
    deque<PCB*> rrQueue;    // ʱ��Ƭ��ת����
    PCB* runningProcess; //��ǰ�������н���
    

    int nextPid;
    int timeSlice;
    Logger* logger;
    SchedulePolicy policy;



    //��ӻ��������̼߳�ر��� 
    std::mutex outputMutex;
    std::atomic<bool> isMonitorRunning;
    std::thread timeSliceMonitorThread;


    //�̳߳���ر���
    ThreadPool thread_Pool;

public:
    ProcessManager(SchedulePolicy p = SchedulePolicy::PRIORITY) : policy(p), isMonitorRunning(false) {
        nextPid = 0;
        runningProcess = nullptr;
        timeSlice = 1;
        logger = Logger::getInstance();
    }

    bool hasProcesses() const {
        return !processMap.empty();
    }

    bool checkAndHandleTimeSlice();
    int createProcess(string name, int priority, int operaTime, std::function<void()> func);
    void terminateProcess(int pid); //��ֹ�ն�
    void wakeupProcess(int pid); //�����ն�
    void blockProcess(int pid); //�����ն�

    void dispatcher();

    void timeSliceExpired(int outpid);
    void addToReadyQueue(int pid);


    void timeSliceMonitorFunc();
    void startTimeSliceMonitor();
    void stopTimeSliceMonitor();


    std::mutex& getOutputMutex() {
        return outputMutex;
    }
    // �������������ProcessManager��
    ~ProcessManager() {
        stopTimeSliceMonitor();
        //// ��ֹͣ��ʱ��
        //if (scheduleTimer) {
        //    scheduleTimer->stop();
        //    delete scheduleTimer;
        //    scheduleTimer = nullptr;
        //}

        // Ȼ���������н���
        for (auto& pair : processMap) {
            delete pair.second;
        }
        processMap.clear();
    }
};

#endif