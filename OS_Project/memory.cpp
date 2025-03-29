#include "memory.h"

//#define DEBUG

void Memory::memoryControl()
{
	system("cls");

	bool cycleFlag = true;
	cin.get();//清除主函数中未处理的换行符

	while (cycleFlag) {
		cout << "[" << path << "]>";
		getline(cin, input);
		inputPos = input.begin();
		findFirstCommand();
		if (!command.empty())
		{

#ifndef DEBUG
			cout << "输入的命令为:" << command << endl;
#endif // !DEBUG

			if (command=="cd")//路径命令，~号代表基地址，使用\分隔文件
			{
				commandChangePath();
				continue;
			}

		}
		else
		{

#ifndef DEBUG
			cout << "换行" << endl;
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
	}//跳过空格
	for (;inputPos != input.end()&&*inputPos!=' '; inputPos++)
	{
		tmp += *inputPos;
	}
	command = tmp;

//	pos = input.find(" ");//未找到会返回-1
}

void Memory::commandChangePath()
{
}
