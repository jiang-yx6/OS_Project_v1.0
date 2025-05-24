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
    SJF, // 短作业优先
    FCFS, //先来先服务
    RR //时间片轮转
};
class Timer {
public:
    atomic<bool> time_slice_expired;  // 使用原子变量来保证线程安全
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
        // 在锁之外join线程，避免死锁
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
    int operaTime; //进程执行总时间
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


    void decrementRemainTime() { if (remainTime > 0) remainTime--; } //减少时间片长度的时间

    bool isFinished() { return remainTime <= 0; }
};
struct Comparator {
    bool operator()(PCB* p1, PCB* p2) {
        return p1->getPriority() > p2->getPriority(); //值越小优先级越高
    }
};
struct SJFComparator {
    bool operator()(PCB* p1, PCB* p2) {
        return p1->getRemainTime() > p2->getRemainTime();  // 剩余时间越短优先级越高
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

    priority_queue<PCB*, vector<PCB*>, Comparator> readyQueue; //就绪队列
    priority_queue<PCB*, vector<PCB*>, SJFComparator> sjfQueue; // 短作业优先队列
    queue<PCB*> fcfsQueue; // FCFS 队列
    queue<PCB*> blockQueue; //阻塞队列
    deque<PCB*> rrQueue;    // 时间片轮转队列
    PCB* runningProcess; //当前正在运行进程
    

    int nextPid;
    int timeSlice;
    Logger* logger;
    SchedulePolicy policy;



    //添加互斥锁和线程监控变量 
    std::mutex outputMutex;
    std::atomic<bool> isMonitorRunning;
    std::thread timeSliceMonitorThread;


    //线程池相关变量
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
    void terminateProcess(int pid); //终止终端
    void wakeupProcess(int pid); //唤醒终端
    void blockProcess(int pid); //阻塞终端

    void dispatcher();

    void timeSliceExpired(int outpid);
    void addToReadyQueue(int pid);


    void timeSliceMonitorFunc();
    void startTimeSliceMonitor();
    void stopTimeSliceMonitor();


    std::mutex& getOutputMutex() {
        return outputMutex;
    }
    // 添加析构函数到ProcessManager类
    ~ProcessManager() {
        stopTimeSliceMonitor();
        //// 先停止定时器
        //if (scheduleTimer) {
        //    scheduleTimer->stop();
        //    delete scheduleTimer;
        //    scheduleTimer = nullptr;
        //}

        // 然后清理所有进程
        for (auto& pair : processMap) {
            delete pair.second;
        }
        processMap.clear();
    }
};

#endif