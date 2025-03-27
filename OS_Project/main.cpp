/*---------- main.c ----------*/
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <thread>       // 为 this_thread 添加头文件
#include <chrono>  
#include "process/process.h"
#define TIME_QUANTUM 4

void test() {
    ProcessManager pm;

    // 创建几个测试进程
    pm.createProcess("Process1", 1, 5);  // 优先级1，需要执行5秒
    pm.createProcess("Process2", 2, 3);  // 优先级2，需要执行3秒
    pm.createProcess("Process3", 3, 4);  // 优先级3，需要执行4秒

    // 让主程序运行一段时间以观察进程调度
    this_thread::sleep_for(chrono::seconds(15));
}
int main() {
    test();
    return 0;
}
