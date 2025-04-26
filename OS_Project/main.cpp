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

void processTest() {
    ProcessManager pm;

    // 创建几个测试进程
    pm.createProcess("Process1", 3, 5, [] {cout << "Process1 hhh" << endl; });  // 优先级1，需要执行5秒
    pm.createProcess("Process2", 2, 3, [] {cout << "Process2 hhh" << endl; });  // 优先级2，需要执行3秒
    pm.createProcess("Process3", 1, 4, [] {cout << "Process3 hhh" << endl; });  // 优先级3，需要执行4秒
    while (pm.hasProcesses()) {

        pm.checkAndHandleTimeSlice();
        this_thread::sleep_for(chrono::milliseconds(100));

    }
    // 让主程序运行一段时间以观察进程调度
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

int main() {
    bool cycleFlag = true;

    while(cycleFlag){
        cout <<
            "选择测试内容：\n\
        1:进程调度\n\
        2:文件管理\n\
        3:内存管理\n\
        0:退出" << endl;
        int input;
        cin >> input;
        switch (input)
        {
        case 1:
            processTest();
            break;
        case 2:
            fileTest();
            break;
        case 3:
            MemoryManagerTest();
            break;
        case 0:
            cycleFlag=false;//输入中文也会返回0
        default:
            break;
        }
    }
    return 0;
}


