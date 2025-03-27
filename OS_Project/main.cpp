/*---------- main.c ----------*/
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <thread>       // Ϊ this_thread ���ͷ�ļ�
#include <chrono>  
#include "process/process.h"
#define TIME_QUANTUM 4

void test() {
    ProcessManager pm;

    // �����������Խ���
    pm.createProcess("Process1", 1, 5);  // ���ȼ�1����Ҫִ��5��
    pm.createProcess("Process2", 2, 3);  // ���ȼ�2����Ҫִ��3��
    pm.createProcess("Process3", 3, 4);  // ���ȼ�3����Ҫִ��4��

    // ������������һ��ʱ���Թ۲���̵���
    this_thread::sleep_for(chrono::seconds(15));
}
int main() {
    test();
    return 0;
}
