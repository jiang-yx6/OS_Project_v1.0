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
         //    file.loginIn(); // loginIn 内部有自己的循环
         //   // 如果 loginIn 正常退出（用户输入 "exit"），那么 g_is_program_running 仍为 true
         //   // 如果是 Ctrl+C 导致 loginIn 提前退出，那么 g_is_program_running 会是 false
         //   if (!g_is_program_running.load()) {
         //       break; // 收到 Ctrl+C，退出 mainControl 循环
         //   }
         //   // 用户从 loginIn 退出后，可以重新进入 loginIn
         //   // 或者在这里添加其他顶层命令


		file.loginIn();

	}



};

