#include "memory.h"

//#define DEBUG

void Memory::memoryControl()
{
	system("cls");

	bool cycleFlag = true;
	cin.get();//�����������δ����Ļ��з�

	while (cycleFlag) {
		cout << "[" << path << "]>";
		getline(cin, input);
		inputPos = input.begin();
		findFirstCommand();
		if (!command.empty())
		{

#ifndef DEBUG
			cout << "���������Ϊ:" << command << endl;
#endif // !DEBUG

			if (command=="cd")//·�����~�Ŵ������ַ��ʹ��\�ָ��ļ�
			{
				commandChangePath();
				continue;
			}

		}
		else
		{

#ifndef DEBUG
			cout << "����" << endl;
#endif // !DEBUG

		}

	}
	
}

void Memory::findFirstCommand()
{
	int pos;
	string tmp = "";
	//string::iterator pos = input.begin();
	while (inputPos!=input.end()&&*inputPos==' ')
	{
		inputPos++;
	}//�����ո�
	for (;inputPos != input.end()&&*inputPos!=' '; inputPos++)
	{
		tmp += *inputPos;
	}
	command = tmp;

//	pos = input.find(" ");//δ�ҵ��᷵��-1
}

void Memory::commandChangePath()
{
}
