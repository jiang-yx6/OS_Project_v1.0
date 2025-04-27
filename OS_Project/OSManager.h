#pragma once
#include "file.h"
#include<iostream>
#include "process/process.h"
class OSManager
{
public:
	File file;
	ProcessManager pm;

	void fileControl()
	{
		system("cls");
		file.loginIn();//登录
		file.loadMainPath();//要求
		cin.get();//清除主函数中未处理的换行符
		system("cls");
		cout << "操作方式：\n\
1:cd 路径  路径从~开始为返回主目录，否则从当前目录开始\n\
2:ls 路径  用法同上，但路径可置空，代表查找当前目录\n\
3:mkdir 文件名  在当前目录下创建文件目录\n\
4:rmdir用法同上\n\
5:mkfile 文件名 在当前目录下创建文件\n\
6:permission 文件名 0~3 设置文件权限\n\
7:logout 退出登录\n\
注意文件路径使用\\" << endl;

		while (1) {
			cout << "[" << file.userName << "@" << file.path << "]>";
			getline(cin, file.input);//会正常读取中文字符，处理时可能会出现问题

			file.inputPos = file.input.begin();
			file.findFirstCommand();

			if (!file.command.empty())
			{
				if (file.command == "cd")//路径命令，~号代表基地址，使用\分隔文件
				{
					pm.createProcess("Process1", 3, 5, [&] {file.commandChangePath(); });
					//file.commandChangePath();
					continue;
				}
				if (file.command == "ls")//后跟文件夹名称或路径或置空
				{
					file.commandShowPathFile();
					continue;
				}
				if (file.command == "mkdir")//后跟文件夹名称
				{
					file.commandCreatePath();
					continue;
				}
				if (file.command == "rmdir")
				{
					file.commandDeletePath();
				}
				if (file.command == "mkfile")
				{
					file.commandCreateFile();
				}
				if (file.command == "permission")
				{
					file.commandChangePermission();
				}
				if (file.command == "logout")
				{
					system("cls");
					file.ioFile.close();
					file.loginIn();//登录
					file.loadMainPath();//要求
					cin.get();//清除主函数中未处理的换行符
					system("cls");
				}
				if (file.command == "exit")//退出程序
				{
					file.ioFile.close();
					return;
				}
			}
		}

	}

	OSManager() {
		auto& timer = pm.getTimer();
		timer.setCallBack([this]() {
			file.commandChangePath();
		});
	}
};

