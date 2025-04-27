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
    SJF, // 短作业优先
    FCFS, //先来先服务
    RR //时间片轮转
};
class PCB {
private:
    int pid;
    string name;
    ProcessState state;
    int priority; //优先级
    int operaTime; //进程执行总时间
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


    void decrementRemainTime() { if (remainTime > 0) remainTime--; } //减少时间片长度的时间

    bool isFinished() { return remainTime <= 0; }
};
class Timer {
private:
    std::thread timer_thread;
    atomic<bool> time_slice_expired;  // 使用原子变量来保证线程安全
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
        // 在锁之外join线程，避免死锁
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
        return p1->getPriority() > p2->getPriority(); //值越小优先级越高
    }
};
struct SJFComparator {
    bool operator()(PCB* p1, PCB* p2) {
        return p1->getRemainTime() > p2->getRemainTime();  // 剩余时间越短优先级越高
    }
};
class ProcessManager {
private:
    unordered_map<int, PCB*> processMap;
    priority_queue<PCB*, vector<PCB*>, Comparator> readyQueue; //就绪队列
    priority_queue<PCB*, vector<PCB*>, SJFComparator> sjfQueue; // 短作业优先队列
    queue<PCB*> blockQueue; //阻塞队列
    queue<PCB*> fcfsQueue; // FCFS 队列
    deque<PCB*> rrQueue;    // 时间片轮转队列
    PCB* runningProcess; //当前正在运行进程
    int nextPid;
    int timeSlice;
    const int TIME_SLICE_MS = 1000; //设置时间片1秒
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
        std::cout << "【调试】当前调度策略是: "
            << (policy == SchedulePolicy::SJF ? "SJF" :
                policy == SchedulePolicy::FCFS ? "FCFS" :
                policy == SchedulePolicy::RR ? "Round Robin" : "PRIORITY")
            << std::endl;

        //// 设置定时器的回调函数
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
    void terminateProcess(int pid); //终止终端
    void wakeupProcess(int pid); //唤醒终端
    void blockProcess(int pid); //阻塞终端

    void dispatcher();

    void timeSliceExpired();
    void addToReadyQueue(PCB* ptr);
    // 添加析构函数到ProcessManager类
    ~ProcessManager() {
        // 先停止定时器
        if (scheduleTimer) {
            scheduleTimer->stop();
            delete scheduleTimer;
            scheduleTimer = nullptr;
        }

        // 然后清理所有进程
        for (auto& pair : processMap) {
            delete pair.second;
        }
        processMap.clear();
    }
};
#endif