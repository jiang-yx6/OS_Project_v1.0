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
#include <cstring>

// �޸ĺ�� processTest ����
void processTest(SchedulePolicy policy) {
    ProcessManager pm(policy);  // ʹ�ô���ĵ��Ȳ���

    // �����������Խ���
    pm.createProcess("Process1", 3, 5, [] {cout << "Process1 hhh" << endl; });  // ���ȼ�1����Ҫִ��5��
    pm.createProcess("Process2", 2, 3, [] {cout << "Process2 hhh" << endl; });  // ���ȼ�2����Ҫִ��3��
    pm.createProcess("Process3", 1, 4, [] {cout << "Process3 hhh" << endl; });  // ���ȼ�3����Ҫִ��4��

    while (pm.hasProcesses()) {
        pm.checkAndHandleTimeSlice();
        this_thread::sleep_for(chrono::milliseconds(50));  // ÿ50������һ��
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
        // ���Ի�ȡ�߼���� 0 �����ݣ�ƫ���� 10
        char data = manager.getData(0, 10);
        std::cout << "Data at logical block 0, offset 10: " << data << "\n";
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
    }

    // ��ӡ�ڴ�״̬
    manager.printMemoryState();

    // �ͷ�����ҳ�� 0
    manager.freeMemory(0);

    // �ٴδ�ӡ�ڴ�״̬
    manager.printMemoryState();

    return;
}

//int main() {
//    bool cycleFlag = true;
//
//    while (cycleFlag) {
//        cout <<
//            "ѡ��������ݣ�\n\
//        1:���̵���\n\
//        2:�ļ�����\n\
//        3:�ڴ����\n\
//        0:�˳�" << endl;
//        int input;
//        cin >> input;
//        switch (input)
//        {
//        case 1: {
//            // ��չ�����㷨ѡ��˵�
//            cout << "ѡ������㷨��\n\
//            1: ���ȼ�����\n\
//            2: ����ҵ���ȣ�SJF��\n\
//            3: �����ȷ���FCFS��\n\
//            4: ʱ��Ƭ��ת��Round Robin��" << endl;
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
//                cout << "��Ч��ѡ��Ĭ��ʹ�����ȼ����ȡ�" << endl;
//                policy = SchedulePolicy::PRIORITY;
//                break;
//            }
//            processTest(policy);  // ����ѡ��ĵ��Ȳ���
//            break;
//        }
//        case 2:
//            fileTest();
//            break;
//        case 3:
//            MemoryManagerTest();
//            break;
//        case 0:
//            cycleFlag = false;//��������Ҳ�᷵��0
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
