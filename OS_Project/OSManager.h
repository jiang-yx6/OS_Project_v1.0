#pragma once
#include "file.h"
#include<iostream>
#include "process/process.h"
class OSManager
{
public:
	File file;
	ProcessManager pm;

	void mainControl() {

		file.loginIn();

		/*//cin.get();//清除主函数中未处理的换行符

		//system("cls");
		bool cycleFlag;
		file.loadMainPath();//要求

		cycleFlag = true;
		cout << "操作方式：\n\
1:cd 路径  路径从~开始为返回主目录，否则从当前目录开始\n\
2:ls 路径  用法同上，但路径可置空，代表查找当前目录\n\
3:mkdir 文件名  在当前目录下创建文件夹\n\
4:rmdir用法同上\n\
注意文件路径使用\\" << endl;

		while (cycleFlag) {

			string str;
			this_thread::sleep_for(chrono::milliseconds(1000));  // 每50毫秒检查一次
			pm.checkAndHandleTimeSlice();
			cout << "[" << file.path << "]>";
			getline(cin, str);//会正常读取中文字符，处理时可能会出现问题
			file.input = str;
			file.inputPos = file.input.begin();
			file.findFirstCommand();

			if (!file.command.empty())
			{
				if (file.command == "cd")//路径命令，~号代表基地址，使用\分隔文件
				{
					string currentInput = file.input;  // 保存当前输入
					pm.createProcess("cd", 1, 1, [this, currentInput]() {
						file.commandChangePath();
						});
					continue;
				}
				if (file.command == "ls")//后跟文件夹名称或路径或置空
				{
					string currentInput = file.input;  // 保存当前输入
					pm.createProcess("cd", 1, 1, [this, currentInput]() {
						file.commandShowPathFile();
						});
					continue;
				}
				if (file.command == "mkdir")//后跟文件夹名称
				{
					string currentInput = file.input;  // 保存当前输入
					pm.createProcess("cd", 1, 1, [this, currentInput]() {
						file.commandCreatePath();
						});
					continue;
				}
				if (file.command == "rmdir")
				{
					string currentInput = file.input;  // 保存当前输入
					pm.createProcess("cd", 1, 1, [this, currentInput]() {
						file.commandDeletePath();
						});
					continue;
				}
				if (file.command == "exit")//退出程序
				{
					file.ioFile.close();
					return;
				}
			}

		}*/
	}

};

