/*---------- main.c ----------*/
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <thread>       // Ϊ this_thread ���ͷ�ļ�
#include <chrono>  
#include "process/process.h"
#define TIME_QUANTUM 4

void processTest() {
    ProcessManager pm;

    // �����������Խ���
    pm.createProcess("Process1", 3, 5);  // ���ȼ�1����Ҫִ��5��
    pm.createProcess("Process2", 2, 3);  // ���ȼ�2����Ҫִ��3��
    pm.createProcess("Process3", 1, 4);  // ���ȼ�3����Ҫִ��4��
    while (pm.hasProcesses()) {

        pm.checkAndHandleTimeSlice();
        this_thread::sleep_for(chrono::milliseconds(100));

    }
    // ������������һ��ʱ���Թ۲���̵���
}
int main() {
    bool cycleFlag = true;
    while(cycleFlag){
        cout <<
            "ѡ��������ݣ�\n\
        1:���̵���\n\
        0:�˳�" << endl;
        int input;
        cin >> input;
        switch (input)
        {
        case 1:
            processTest();
        case 0:
            cycleFlag=false;
        default:
            break;
        }
    }
    return 0;
}
