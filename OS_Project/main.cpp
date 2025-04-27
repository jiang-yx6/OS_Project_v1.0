/*---------- main.c ----------*/
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <thread>       // 为 this_thread 添加头文件
#include <chrono>  
#include "process/process.h"
#include "file.h"
#include"MemoryManager.h"
#include"OSManager.h"
#define TIME_QUANTUM 4
#include <cstring>

// 修改后的 processTest 函数
void processTest(SchedulePolicy policy) {
    ProcessManager pm(policy);  // 使用传入的调度策略

    // 创建几个测试进程
    pm.createProcess("Process1", 3, 5, [] {cout << "Process1 hhh" << endl; });  // 优先级1，需要执行5秒
    pm.createProcess("Process2", 2, 3, [] {cout << "Process2 hhh" << endl; });  // 优先级2，需要执行3秒
    pm.createProcess("Process3", 1, 4, [] {cout << "Process3 hhh" << endl; });  // 优先级3，需要执行4秒

    while (pm.hasProcesses()) {
        pm.checkAndHandleTimeSlice();
        this_thread::sleep_for(chrono::milliseconds(50));  // 每50毫秒检查一次
    }
}


void fileTest()
{
    OSManager os;
    while (true) {
        os.file.fileControl2();
    }
    /*os.file.fileControl();*/
    //pm.createProcess("Process1", 3, 5);
    /*File fc;
    fc.fileControl();*/
}
void MemoryManagerTest() {
    MemoryManager manager(MemoryManager::ReplacementAlgorithm::FIFO);

    try {
        // 尝试获取逻辑块号 0 的数据，偏移量 10
        char data = manager.getData(0, 10);
        std::cout << "Data at logical block 0, offset 10: " << data << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    // 打印内存状态
    manager.printMemoryState();

    // 释放虚拟页号 0
    manager.freeMemory(0);

    // 再次打印内存状态
    manager.printMemoryState();

    return;
}

//int main() {
//    bool cycleFlag = true;
//
//    while (cycleFlag) {
//        cout <<
//            "选择测试内容：\n\
//        1:进程调度\n\
//        2:文件管理\n\
//        3:内存管理\n\
//        0:退出" << endl;
//        int input;
//        cin >> input;
//        switch (input)
//        {
//        case 1: {
//            // 扩展调度算法选择菜单
//            cout << "选择调度算法：\n\
//            1: 优先级调度\n\
//            2: 短作业优先（SJF）\n\
//            3: 先来先服务（FCFS）\n\
//            4: 时间片轮转（Round Robin）" << endl;
//            int alg;
//            cin >> alg;
//
//            SchedulePolicy policy;
//            switch (alg) {
//            case 1:
//                policy = SchedulePolicy::PRIORITY;
//                break;
//            case 2:
//                policy = SchedulePolicy::SJF;
//                break;
//            case 3:
//                policy = SchedulePolicy::FCFS;
//                break;
//            case 4:
//                policy = SchedulePolicy::RR;
//                break;
//            default:
//                cout << "无效的选择，默认使用优先级调度。" << endl;
//                policy = SchedulePolicy::PRIORITY;
//                break;
//            }
//            processTest(policy);  // 传入选择的调度策略
//            break;
//        }
//        case 2:
//            fileTest();
//            break;
//        case 3:
//            MemoryManagerTest();
//            break;
//        case 0:
//            cycleFlag = false;//输入中文也会返回0
//        default:
//            break;
//        }
//    }
//    return 0;
//}

int main() {
    OSManager* os = new OSManager();
    os->mainControl();
    return 0;
}
