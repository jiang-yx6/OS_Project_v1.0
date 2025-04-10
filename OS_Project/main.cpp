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

void processTest() {
    ProcessManager pm;

    // 创建几个测试进程
    pm.createProcess("Process1", 3, 5);  // 优先级1，需要执行5秒
    pm.createProcess("Process2", 2, 3);  // 优先级2，需要执行3秒
    pm.createProcess("Process3", 1, 4);  // 优先级3，需要执行4秒
    while (pm.hasProcesses()) {

        pm.checkAndHandleTimeSlice();
        this_thread::sleep_for(chrono::milliseconds(100));

    }
    // 让主程序运行一段时间以观察进程调度
}

void fileTest()
{
    OSManager os;
    os.file.fileControl();
    
    /*File fc;
    fc.fileControl();*/
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
            Memorytest();
            break;
        case 0:
            cycleFlag=false;//输入中文也会返回0
        default:
            break;
        }
    }
    return 0;
}
