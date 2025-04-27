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
		file.loginIn();//��¼
		file.loadMainPath();//Ҫ��
		cin.get();//�����������δ����Ļ��з�
		system("cls");
		cout << "������ʽ��\n\
1:cd ·��  ·����~��ʼΪ������Ŀ¼������ӵ�ǰĿ¼��ʼ\n\
2:ls ·��  �÷�ͬ�ϣ���·�����ÿգ�������ҵ�ǰĿ¼\n\
3:mkdir �ļ���  �ڵ�ǰĿ¼�´����ļ�Ŀ¼\n\
4:rmdir�÷�ͬ��\n\
5:mkfile �ļ��� �ڵ�ǰĿ¼�´����ļ�\n\
6:permission �ļ��� 0~3 �����ļ�Ȩ��\n\
7:logout �˳���¼\n\
ע���ļ�·��ʹ��\\" << endl;

		while (1) {
			cout << "[" << file.userName << "@" << file.path << "]>";
			getline(cin, file.input);//��������ȡ�����ַ�������ʱ���ܻ��������

			file.inputPos = file.input.begin();
			file.findFirstCommand();

			if (!file.command.empty())
			{
				if (file.command == "cd")//·�����~�Ŵ������ַ��ʹ��\�ָ��ļ�
				{
					pm.createProcess("Process1", 3, 5, [&] {file.commandChangePath(); });
					//file.commandChangePath();
					continue;
				}
				if (file.command == "ls")//����ļ������ƻ�·�����ÿ�
				{
					file.commandShowPathFile();
					continue;
				}
				if (file.command == "mkdir")//����ļ�������
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
					file.loginIn();//��¼
					file.loadMainPath();//Ҫ��
					cin.get();//�����������δ����Ļ��з�
					system("cls");
				}
				if (file.command == "exit")//�˳�����
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

