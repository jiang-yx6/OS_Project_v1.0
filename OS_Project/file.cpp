#include "file.h"

//#define DEBUG

void File::fileControl()
{
	system("cls");

	bool cycleFlag = true;
	cin.get();//清除主函数中未处理的换行符
	loadPath();

	while (cycleFlag) {
		cout << "[" << path << "]>";
		getline(cin, input);//会正常读取中文字符，处理时可能会出现问题
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

void File::findFirstCommand()
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

void File::loadPath()
{
	//文件命名为storage
	ioFile.open("storage.txt",ios::binary|ios::in|ios::out);
	if (!ioFile.is_open())
	{
		ioFile.open("storage.txt",ios::out);//fstream需已只写模式创建文件
		ioFile.close();
		ioFile.open("storage.txt", ios::binary | ios::in | ios::out);
		fileInitialize();
		cout << "文件创建成功" << endl;
	}



	//char reader[1024];//seek越界不会提示，如果读取出界则在reader中原有的数据不会改变
	/*if (ioFile.is_open())
	{
		cout << "";
	}*/
}

void File::commandChangePath()
{
}

void File::fileInitialize()
{
	memPos = 0;
	addClock();


}

void File::addClock()
{
	placeHold(CLOCK_SIZE);

	cout << "指针位置检查" << endl;
}

void File::placeHold(int num)
{
	while (num > 0)
	{
		writeMem(0);
		memPos++;
		num--;
	}
}

void File::writeMem(int num)
{
	//0~255
	ioFile.seekp(memPos * 3);
	int tmp1, tmp2;
	tmp1 = num / 16;
	tmp2 = num % 16;
	num = 2;
	while(num){
		switch (tmp1)
		{
		case 10:
			ioFile.write("A", 1);
			break;
		case 11:
			ioFile.write("B", 1);
			break;
		case 12:
			ioFile.write("C", 1);
			break;
		case 13:
			ioFile.write("D", 1);
			break;
		case 14:
			ioFile.write("E", 1);
			break;
		case 15:
			ioFile.write("F", 1);
			break;
		default:
			ioFile.write(to_string(tmp1).c_str(), 1);
			break;
		}
		tmp1 = tmp2;
		num--;
	}
	ioFile.write(" ", 1);
}
