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

		/*//cin.get();//�����������δ����Ļ��з�

		//system("cls");
		bool cycleFlag;
		file.loadMainPath();//Ҫ��

		cycleFlag = true;
		cout << "������ʽ��\n\
1:cd ·��  ·����~��ʼΪ������Ŀ¼������ӵ�ǰĿ¼��ʼ\n\
2:ls ·��  �÷�ͬ�ϣ���·�����ÿգ�������ҵ�ǰĿ¼\n\
3:mkdir �ļ���  �ڵ�ǰĿ¼�´����ļ���\n\
4:rmdir�÷�ͬ��\n\
ע���ļ�·��ʹ��\\" << endl;

		while (cycleFlag) {

			string str;
			this_thread::sleep_for(chrono::milliseconds(1000));  // ÿ50������һ��
			pm.checkAndHandleTimeSlice();
			cout << "[" << file.path << "]>";
			getline(cin, str);//��������ȡ�����ַ�������ʱ���ܻ��������
			file.input = str;
			file.inputPos = file.input.begin();
			file.findFirstCommand();

			if (!file.command.empty())
			{
				if (file.command == "cd")//·�����~�Ŵ������ַ��ʹ��\�ָ��ļ�
				{
					string currentInput = file.input;  // ���浱ǰ����
					pm.createProcess("cd", 1, 1, [this, currentInput]() {
						file.commandChangePath();
						});
					continue;
				}
				if (file.command == "ls")//����ļ������ƻ�·�����ÿ�
				{
					string currentInput = file.input;  // ���浱ǰ����
					pm.createProcess("cd", 1, 1, [this, currentInput]() {
						file.commandShowPathFile();
						});
					continue;
				}
				if (file.command == "mkdir")//����ļ�������
				{
					string currentInput = file.input;  // ���浱ǰ����
					pm.createProcess("cd", 1, 1, [this, currentInput]() {
						file.commandCreatePath();
						});
					continue;
				}
				if (file.command == "rmdir")
				{
					string currentInput = file.input;  // ���浱ǰ����
					pm.createProcess("cd", 1, 1, [this, currentInput]() {
						file.commandDeletePath();
						});
					continue;
				}
				if (file.command == "exit")//�˳�����
				{
					file.ioFile.close();
					return;
				}
			}

		}*/
	}

};

