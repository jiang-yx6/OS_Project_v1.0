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

class PCB; // 告诉编译器 PCB 是一个类，它的完整定义在后面

enum class SchedulePolicy {
    PRIORITY,
    SJF, // 短作业优先
    FCFS, //先来先服务
    RR //时间片轮转
};
/*
class Timer {
public:
    PCB* associatedPCB = nullptr; // 在PCB构造时设置
    atomic<bool> time_slice_expired;  // 使用原子变量来保证线程安全
    bool running;
    function<void()>callback;
    // 新增：I/O 阻塞标志
    atomic<bool> is_io_blocked_flag; // 用于向ProcessManager传递I/O阻塞信号

   

public:
    Timer() : running(false), time_slice_expired(false) {}

    ~Timer() {
        //stop();
    }

    void setCallBack(function<void()>cb) {
        callback = std::move(cb);
    }

    // 新增一个设置关联PCB的方法
    void setAssociatedPCB(PCB* pcb) {
        associatedPCB = pcb;
    }


    void start(int milliseconds) {
        stop();
        running = true;
        time_slice_expired = false;
        is_io_blocked_flag = false; // 重置I/O阻塞标志

        while (running)
        {
            if (associatedPCB && associatedPCB->isIoIntensive) {
                // 模拟 I/O 阻塞：如果进程是I/O密集型，有一定概率提前结束时间片并阻塞
                if (rand() % 100 < 30) { // 30% 概率触发 I/O 阻塞
                    cout << "[PID " << associatedPCB->pid << "] " << associatedPCB->name << " simulated I/O blocking (Timer triggered)!" << endl;
                    is_io_blocked_flag = true; // 设置I/O阻塞标志
                    time_slice_expired = true; // 依然标记时间片结束，但原因不同
                    running = false; // 立即停止
                    return; // 提前返回，不进行sleep
                }
            }

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

    // 新增：检查是否因I/O阻塞而结束
    bool isIoBlocked() {
        return is_io_blocked_flag.load(); // 使用.load()访问原子变量
    }

    void resetTimeSliceFlag() {
        time_slice_expired = false;
        is_io_blocked_flag = false; // 同时重置I/O阻塞标志
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
    bool isIoIntensive = false; // 默认不进行I/O阻塞，创建时根据命令设置

    /*int createdTime;
    int startedTime;
    int deleteTime;
    size_t memoryStart;
    size_t memorySize;
    vector<int> allocatedResources;
    vector<int> requestedResources;
    vector<int> openFiles;*/
/*
public:
    PCB(int id, string pname, int pri, int opTime) {
        pid = id;
        name = pname;
        state = READY;
        priority = pri;
        operaTime = opTime;
        remainTime = opTime;
        scheduleTimer = new Timer();
        scheduleTimer->setAssociatedPCB(this); // (2) 立即设置关联的PCB
    }

    void registerFunc(std::function<void()> func) {
        //task = std::move(func);
        //scheduleTimer->setCallBack(task);
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
*/
class Timer {
public:
    PCB* associatedPCB = nullptr; // 声明为指针，这里只需要前向声明
    atomic<bool> time_slice_expired;
    bool running; // 这个成员变量在新的start逻辑中可能不再需要
    function<void()>callback;
    atomic<bool> is_io_blocked_flag;

public:
    // 构造函数
    Timer();
    // 析构函数
    ~Timer();

    void setCallBack(function<void()>cb);
    void setAssociatedPCB(PCB* pcb);

    // !!! 关键：只声明 start 方法，实现放在 .cpp 文件中 !!!
    void start(int milliseconds);

    void stop(); // 这个方法在新逻辑中可能为空或被移除

    bool isTimeSliceExpired();
    bool isIoBlocked();
    void resetTimeSliceFlag();
};

class PCB {
public:
    int pid;
    string name;
    ProcessState state; // 确保 ProcessState 已定义
    int priority;
    int operaTime; //进程执行总时间
    int remainTime;
    std::function<void()> task;
    Timer* scheduleTimer; // 声明为指针，这里只需要 Timer 的前向声明
    bool isIoIntensive = false;

public:
    ~PCB() {
        if (scheduleTimer) {
            delete scheduleTimer;
            scheduleTimer = nullptr;
        }
    }
    // 构造函数
    PCB(int id, string pname, int pri, int opTime);
    // 注册任务回调
    void registerFunc(std::function<void()> func);
    // 获取Timer指针
    Timer* getPTimer();
    // 获取任务回调 (这个方法在当前设计中可能用不到，因为Timer直接调用)
    function<void()> getTask();
    // 获取PID
    int getPid() const;
    // 获取名称
    string getName() const;
    // 获取状态
    ProcessState getState() const;
    // 获取优先级
    int getPriority() const;
    // 获取剩余时间
    int getRemainTime() const;
    // 设置状态
    void setState(ProcessState newState);
    // 减少剩余时间
    void decrementRemainTime();
    // 判断是否完成
    bool isFinished();
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
//class ThreadPool {
//public:
//    int nums = 2; // 线程数量
//    std::atomic<bool> stop_threads; // 新增：线程池停止标志
//
//    ThreadPool() : stop_threads(false) { // 初始化停止标志
//        for (int i = 0; i < nums; i++) {
//            threads.emplace_back([this] {
//                while (true) { // 循环直到被告知停止
//                    std::unique_lock<std::mutex> lock(runningQueueMutex);
//                    // 只有在停止线程或有任务时才唤醒
//                    condition.wait(lock, [this] {
//                        return stop_threads.load() || !noComsumed_runningProcess.empty();
//                        });
//
//                    if (stop_threads.load() && noComsumed_runningProcess.empty()) {
//                        // 如果收到停止信号且没有待处理任务，则退出线程
//                        //Logger::getInstance()->log("Thread Pool Worker exiting."); // <--- 改用 Logger
//                        return;
//                    }
//
//                    //Logger::getInstance()->log("Consumer Thread got runningQueueMutex"); // <--- 改用 Logger (可选，调试信息)
//                    //Logger::getInstance()->log("Process remain time: " + std::to_string(noComsumed_runningProcess.front()->getRemainTime())); // <--- 改用 Logger (可选，调试信息)
//                    //Logger::getInstance()->log("1 noComsumed Queue Size: " + std::to_string(noComsumed_runningProcess.size())); // <--- 改用 Logger (可选，调试信息)
//
//                    PCB* pcb_to_run = noComsumed_runningProcess.front();
//                    comsumed_runningProcess.push(pcb_to_run); // 将任务移到已消费队列
//                    //Logger::getInstance()->log("ComsumedQueue Queue Size: " + std::to_string(comsumed_runningProcess.size())); // <--- 改用 Logger (可选，调试信息)
//                    noComsumed_runningProcess.pop();
//                    //Logger::getInstance()->log("2 noComsumed Queue Size: " + std::to_string(noComsumed_runningProcess.size())); // <--- 改用 Logger (可选，调试信息)
//                    lock.unlock(); // 释放锁，允许其他线程访问队列
//
//                    // 执行任务，Timer::start 内部会包含 sleep_for
//                    Timer* timer = pcb_to_run->getPTimer();
//                    timer->start(TIME_SLICE_MS); // 注意：这里是阻塞调用
//                }
//                });
//        }
//    }

//    void enqueue(PCB* readyToRunning) {
//        {
//            //Logger::getInstance()->log("Producer Thread trying to get runningQueueMutex"); // <--- 改用 Logger (可选，调试信息)
//            std::unique_lock<std::mutex> lock(runningQueueMutex);
//            //Logger::getInstance()->log("Producer Thread got runningQueueMutex"); // <--- 改用 Logger (可选，调试信息)
//            noComsumed_runningProcess.emplace(readyToRunning);
//            curRunningLen++; // 标记为正在运行的进程
//        }
//        condition.notify_all(); // 唤醒所有等待的消费者线程
//    }
//
//    vector<thread> threads;
//    queue<PCB*> noComsumed_runningProcess; // 待消费（待运行）的进程队列
//    queue<PCB*> comsumed_runningProcess; // 已消费（已运行完时间片）的进程，等待ProcessManager处理的队列
//    std::atomic<int> curRunningLen = 0; // 当前运行中的进程数量 (由线程池管理)
//    std::mutex runningQueueMutex; // 保护两个队列的互斥锁
//    std::condition_variable condition; // 用于消费者线程的条件变量
//
//    // 析构函数：确保所有线程安全退出
//    ~ThreadPool() {
//        stop_threads = true; // 设置停止标志
//        condition.notify_all(); // 唤醒所有等待的线程
//        for (auto& thread : threads) {
//            if (thread.joinable()) {
//                thread.join(); // 等待所有线程完成
//            }
//        }
//        //Logger::getInstance()->log("ThreadPool destroyed."); // <--- 改用 Logger
//    }
//    // removed: mutex& getMutex() { return runningQueueMutex; } // 这个方法已不再需要
//};

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

    std::mutex pmMutex; // 新增：ProcessManager 内部状态的互斥锁，保护所有队列和map
    std::atomic<bool> isMonitorRunning;
    std::thread timeSliceMonitorThread;


    //线程池相关变量
    ThreadPool thread_Pool;

public:
    ProcessManager(SchedulePolicy p = SchedulePolicy::SJF) : policy(p), isMonitorRunning(false) {
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

    //暂时删除
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