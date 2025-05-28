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

class PCB; // ���߱����� PCB ��һ���࣬�������������ں���

enum class SchedulePolicy {
    PRIORITY,
    SJF, // ����ҵ����
    FCFS, //�����ȷ���
    RR //ʱ��Ƭ��ת
};
/*
class Timer {
public:
    PCB* associatedPCB = nullptr; // ��PCB����ʱ����
    atomic<bool> time_slice_expired;  // ʹ��ԭ�ӱ�������֤�̰߳�ȫ
    bool running;
    function<void()>callback;
    // ������I/O ������־
    atomic<bool> is_io_blocked_flag; // ������ProcessManager����I/O�����ź�

   

public:
    Timer() : running(false), time_slice_expired(false) {}

    ~Timer() {
        //stop();
    }

    void setCallBack(function<void()>cb) {
        callback = std::move(cb);
    }

    // ����һ�����ù���PCB�ķ���
    void setAssociatedPCB(PCB* pcb) {
        associatedPCB = pcb;
    }


    void start(int milliseconds) {
        stop();
        running = true;
        time_slice_expired = false;
        is_io_blocked_flag = false; // ����I/O������־

        while (running)
        {
            if (associatedPCB && associatedPCB->isIoIntensive) {
                // ģ�� I/O ���������������I/O�ܼ��ͣ���һ��������ǰ����ʱ��Ƭ������
                if (rand() % 100 < 30) { // 30% ���ʴ��� I/O ����
                    cout << "[PID " << associatedPCB->pid << "] " << associatedPCB->name << " simulated I/O blocking (Timer triggered)!" << endl;
                    is_io_blocked_flag = true; // ����I/O������־
                    time_slice_expired = true; // ��Ȼ���ʱ��Ƭ��������ԭ��ͬ
                    running = false; // ����ֹͣ
                    return; // ��ǰ���أ�������sleep
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
        // ����֮��join�̣߳���������
    }

    bool isTimeSliceExpired() {
        return time_slice_expired;
    }

    // ����������Ƿ���I/O����������
    bool isIoBlocked() {
        return is_io_blocked_flag.load(); // ʹ��.load()����ԭ�ӱ���
    }

    void resetTimeSliceFlag() {
        time_slice_expired = false;
        is_io_blocked_flag = false; // ͬʱ����I/O������־
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
    bool isIoIntensive = false; // Ĭ�ϲ�����I/O����������ʱ������������

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
        scheduleTimer->setAssociatedPCB(this); // (2) �������ù�����PCB
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


    void decrementRemainTime() { if (remainTime > 0) remainTime--; } //����ʱ��Ƭ���ȵ�ʱ��

    bool isFinished() { return remainTime <= 0; }
};
*/
class Timer {
public:
    PCB* associatedPCB = nullptr; // ����Ϊָ�룬����ֻ��Ҫǰ������
    atomic<bool> time_slice_expired;
    bool running; // �����Ա�������µ�start�߼��п��ܲ�����Ҫ
    function<void()>callback;
    atomic<bool> is_io_blocked_flag;

public:
    // ���캯��
    Timer();
    // ��������
    ~Timer();

    void setCallBack(function<void()>cb);
    void setAssociatedPCB(PCB* pcb);

    // !!! �ؼ���ֻ���� start ������ʵ�ַ��� .cpp �ļ��� !!!
    void start(int milliseconds);

    void stop(); // ������������߼��п���Ϊ�ջ��Ƴ�

    bool isTimeSliceExpired();
    bool isIoBlocked();
    void resetTimeSliceFlag();
};

class PCB {
public:
    int pid;
    string name;
    ProcessState state; // ȷ�� ProcessState �Ѷ���
    int priority;
    int operaTime; //����ִ����ʱ��
    int remainTime;
    std::function<void()> task;
    Timer* scheduleTimer; // ����Ϊָ�룬����ֻ��Ҫ Timer ��ǰ������
    bool isIoIntensive = false;

public:
    ~PCB() {
        if (scheduleTimer) {
            delete scheduleTimer;
            scheduleTimer = nullptr;
        }
    }
    // ���캯��
    PCB(int id, string pname, int pri, int opTime);
    // ע������ص�
    void registerFunc(std::function<void()> func);
    // ��ȡTimerָ��
    Timer* getPTimer();
    // ��ȡ����ص� (��������ڵ�ǰ����п����ò�������ΪTimerֱ�ӵ���)
    function<void()> getTask();
    // ��ȡPID
    int getPid() const;
    // ��ȡ����
    string getName() const;
    // ��ȡ״̬
    ProcessState getState() const;
    // ��ȡ���ȼ�
    int getPriority() const;
    // ��ȡʣ��ʱ��
    int getRemainTime() const;
    // ����״̬
    void setState(ProcessState newState);
    // ����ʣ��ʱ��
    void decrementRemainTime();
    // �ж��Ƿ����
    bool isFinished();
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
//class ThreadPool {
//public:
//    int nums = 2; // �߳�����
//    std::atomic<bool> stop_threads; // �������̳߳�ֹͣ��־
//
//    ThreadPool() : stop_threads(false) { // ��ʼ��ֹͣ��־
//        for (int i = 0; i < nums; i++) {
//            threads.emplace_back([this] {
//                while (true) { // ѭ��ֱ������ֹ֪ͣ
//                    std::unique_lock<std::mutex> lock(runningQueueMutex);
//                    // ֻ����ֹͣ�̻߳�������ʱ�Ż���
//                    condition.wait(lock, [this] {
//                        return stop_threads.load() || !noComsumed_runningProcess.empty();
//                        });
//
//                    if (stop_threads.load() && noComsumed_runningProcess.empty()) {
//                        // ����յ�ֹͣ�ź���û�д������������˳��߳�
//                        //Logger::getInstance()->log("Thread Pool Worker exiting."); // <--- ���� Logger
//                        return;
//                    }
//
//                    //Logger::getInstance()->log("Consumer Thread got runningQueueMutex"); // <--- ���� Logger (��ѡ��������Ϣ)
//                    //Logger::getInstance()->log("Process remain time: " + std::to_string(noComsumed_runningProcess.front()->getRemainTime())); // <--- ���� Logger (��ѡ��������Ϣ)
//                    //Logger::getInstance()->log("1 noComsumed Queue Size: " + std::to_string(noComsumed_runningProcess.size())); // <--- ���� Logger (��ѡ��������Ϣ)
//
//                    PCB* pcb_to_run = noComsumed_runningProcess.front();
//                    comsumed_runningProcess.push(pcb_to_run); // �������Ƶ������Ѷ���
//                    //Logger::getInstance()->log("ComsumedQueue Queue Size: " + std::to_string(comsumed_runningProcess.size())); // <--- ���� Logger (��ѡ��������Ϣ)
//                    noComsumed_runningProcess.pop();
//                    //Logger::getInstance()->log("2 noComsumed Queue Size: " + std::to_string(noComsumed_runningProcess.size())); // <--- ���� Logger (��ѡ��������Ϣ)
//                    lock.unlock(); // �ͷ��������������̷߳��ʶ���
//
//                    // ִ������Timer::start �ڲ������ sleep_for
//                    Timer* timer = pcb_to_run->getPTimer();
//                    timer->start(TIME_SLICE_MS); // ע�⣺��������������
//                }
//                });
//        }
//    }

//    void enqueue(PCB* readyToRunning) {
//        {
//            //Logger::getInstance()->log("Producer Thread trying to get runningQueueMutex"); // <--- ���� Logger (��ѡ��������Ϣ)
//            std::unique_lock<std::mutex> lock(runningQueueMutex);
//            //Logger::getInstance()->log("Producer Thread got runningQueueMutex"); // <--- ���� Logger (��ѡ��������Ϣ)
//            noComsumed_runningProcess.emplace(readyToRunning);
//            curRunningLen++; // ���Ϊ�������еĽ���
//        }
//        condition.notify_all(); // �������еȴ����������߳�
//    }
//
//    vector<thread> threads;
//    queue<PCB*> noComsumed_runningProcess; // �����ѣ������У��Ľ��̶���
//    queue<PCB*> comsumed_runningProcess; // �����ѣ���������ʱ��Ƭ���Ľ��̣��ȴ�ProcessManager����Ķ���
//    std::atomic<int> curRunningLen = 0; // ��ǰ�����еĽ������� (���̳߳ع���)
//    std::mutex runningQueueMutex; // �����������еĻ�����
//    std::condition_variable condition; // �����������̵߳���������
//
//    // ����������ȷ�������̰߳�ȫ�˳�
//    ~ThreadPool() {
//        stop_threads = true; // ����ֹͣ��־
//        condition.notify_all(); // �������еȴ����߳�
//        for (auto& thread : threads) {
//            if (thread.joinable()) {
//                thread.join(); // �ȴ������߳����
//            }
//        }
//        //Logger::getInstance()->log("ThreadPool destroyed."); // <--- ���� Logger
//    }
//    // removed: mutex& getMutex() { return runningQueueMutex; } // ��������Ѳ�����Ҫ
//};

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

    std::mutex pmMutex; // ������ProcessManager �ڲ�״̬�Ļ��������������ж��к�map
    std::atomic<bool> isMonitorRunning;
    std::thread timeSliceMonitorThread;


    //�̳߳���ر���
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
    void terminateProcess(int pid); //��ֹ�ն�
    void wakeupProcess(int pid); //�����ն�
    void blockProcess(int pid); //�����ն�

    void dispatcher();

    void timeSliceExpired(int outpid);
    void addToReadyQueue(int pid);


    void timeSliceMonitorFunc();
    void startTimeSliceMonitor();
    void stopTimeSliceMonitor();

    //��ʱɾ��
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