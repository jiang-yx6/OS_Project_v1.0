/*---------- main.c ----------*/
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include <thread>       // Ϊ this_thread ���ͷ�ļ�
#include <chrono>  
#include "process/process.h"
#include "file.h"
#include"MemoryManager.h"
#include"OSManager.h"
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
            "ѡ��������ݣ�\n\
        1:���̵���\n\
        2:�ļ�����\n\
        3:�ڴ����\n\
        0:�˳�" << endl;
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
            cycleFlag=false;//��������Ҳ�᷵��0
        default:
            break;
        }
    }
    return 0;
}
