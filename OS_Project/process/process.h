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
using namespace std;
typedef enum { READY, RUNNING, BLOCKED, OVER } ProcessState;

class PCB {
private:
    int pid;
    string name;
    ProcessState state;
    int priority;
    int operaTime; //����ִ����ʱ��
    int remainTime;
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

    int getPid() const { return pid; }
    string getName() const { return name; }
    ProcessState getState() const { return state; }
    int getPriority() const { return priority; }

    void setState(ProcessState newState) { state = newState; }


    void decrementRemainTime() { if (remainTime > 0) remainTime--; } //����ʱ��Ƭ���ȵ�ʱ��

    bool isFinished() { return remainTime <= 0; }
};

class Timer {
private:
    std::thread timer_thread;
    function<void()> callback;
    bool running;

public:
    Timer() : running(false) {}

    ~Timer() {
        stop();
    }

    void start(int milliseconds) {
        // ��ֹͣ��ǰ�߳�
        stop();

        // Ȼ���������߳�
        {
            running = true;
            timer_thread = thread([this, milliseconds]() {
                while (running) {
                    this_thread::sleep_for(chrono::milliseconds(milliseconds));
                    if (running && callback) {
                        callback();
                    }
                }
                });
        }
    }

    void stop() {
        {
            running = false;
        }

        // ����֮��join�̣߳���������
        if (timer_thread.joinable()) {
            timer_thread.join();
        }
    }

    void setCallback(function<void()> cb) {
        callback = cb;
    }
};

struct Comparator {
    bool operator()(PCB* p1, PCB* p2) {
        return p1->getPriority() > p2->getPriority(); //ֵԽС���ȼ�Խ��
    }
};

class ProcessManager {
private:
    unordered_map<int, PCB*> processMap;
    priority_queue<PCB*,vector<PCB*>, Comparator> readyQueue; //��������
    queue<PCB*> blockQueue; //��������
    PCB* runningProcess; //��ǰ�������н���
    int nextPid;
    int timeSlice;
    const int TIME_SLICE_MS = 1000; //����ʱ��Ƭ1��
    Timer* scheduleTimer;

public:
    ProcessManager() {
        nextPid = 0;
        runningProcess = NULL;
        scheduleTimer = new Timer();
        timeSlice = 1;
        //���ûص�����
        scheduleTimer->setCallback([this]() {
            this->timeSliceExpired();
            });
    }

    int createProcess(string name, int priority, int operaTime);
    void terminateProcess(int pid); //��ֹ�ն�
    void wakeupProcess(int pid); //�����ն�
    void blockProcess(int pid); //�����ն�

    void dispatcher();

    void timeSliceExpired();

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
