#pragma once
#include "file.h"
#include<iostream>
#include "process/process.h"
#include "global_state.h"
class OSManager
{
public:
	File file;
	ProcessManager pm;

	void mainControl() {

         //while (g_is_program_running.load()) {
         //    file.loginIn(); // loginIn �ڲ����Լ���ѭ��
         //   // ��� loginIn �����˳����û����� "exit"������ô g_is_program_running ��Ϊ true
         //   // ����� Ctrl+C ���� loginIn ��ǰ�˳�����ô g_is_program_running ���� false
         //   if (!g_is_program_running.load()) {
         //       break; // �յ� Ctrl+C���˳� mainControl ѭ��
         //   }
         //   // �û��� loginIn �˳��󣬿������½��� loginIn
         //   // �������������������������


		file.loginIn();

	}



};

