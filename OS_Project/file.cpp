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
			if (command=="ls")
			{
				//commandShowPathFile();
			}
			if (command=="mkdir")
			{
				commandCreatePath();
				continue;
			}

			//list<MyFCB>::iterator it = FCBList.begin();

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
	ioFile.open("storage.txt");
	if (!ioFile.is_open())//无文件，重新创建
	{
		ioFile.open("storage.txt",ios::out);//fstream需已只写模式创建文件
		ioFile.close();
		ioFile.open("storage.txt");
		fileInitialize();
		cout << "文件创建成功" << endl;
		ioFile.close();//保存文件
		ioFile.open("storage.txt");
	}

	command = "~";
	locateBlock();

	//char reader[1024];//seek越界不会提示，如果读取出界则在reader中原有的数据不会改变
}

void File::commandChangePath()
{
	command = "";
	findFirstCommand();
	locateBlock();
}

void File::commandCreatePath()
{
}

void File::fileInitialize()
{
	addBlock(0);

	memPos = 1;
	writeMem(1);//标记0号块已使用
}

void File::addBlock(int blockNum)
{
	memPos = BLOCK_SIZE * blockNum;
	placeHold(BLOCK_SIZE);
}

void File::placeHold(int num)
{
	while (num > 0)
	{
		writeMem(0);
		num--;
	}
}

void File::writeMem(int num)
{
	//0~255
	//直观显示模拟内存中数据
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
	//使用16进制ascii码查看器来查看内存内容
	/*ioFile.seekp(memPos);
	ioFile.put((char)num);*/

	memPos++;
}

int File::readMem()
{
	//0~255
	//直观显示模拟内存中数据
	ioFile.seekg(memPos * 3);
	char tmp[1];
	int num=0;
	ioFile.read(tmp, 1);
	while (tmp[0]!=' ') {
		num *= 16;
		switch (tmp[0])
		{
		case 'A':
			num += 10;
			break;
		case 'B':
			num += 11;
			break;
		case 'C':
			num += 12;
			break;
		case 'D':
			num += 13;
			break;
		case 'E':
			num += 14;
			break;
		case 'F':
			num += 15;
			break;
		default:
			num += (int)tmp[0] - 48;
			break;
		}
		ioFile.read(tmp, 1);
	}
	//使用16进制ascii码查看器来查看内存内容
	/*ioFile.seekg(memPos);
	num = (int)ioFile.get();*/

	memPos++;
	return num;
}

int File::locateBlock()
{
	int cutPos=0,cutEnd;
	string tmp;
	cutEnd = command.find('//');
	tmp = command.substr(cutPos,cutEnd);//第二个参数为-1则会读到结尾（无效参数？）
	if (strcmp(tmp.c_str(), "~")==0)
	{
		//表明从根目录起
		loadFCB(0);
	}
	return 0;
}

void File::loadFCB(int blockNum)
{
	FCBList.clear();
	MyFCB tmpFCB;
	memPos = blockNum * BLOCK_SIZE + 2;
	int tmp,nextBlock;
	while(1){
		nextBlock = readMem();
		nextBlock *= 256;
		nextBlock += readMem();
		for (size_t i = 1; i < 4096 / 64; i++)
		{
			memPos = blockNum * BLOCK_SIZE + i * 64;
			tmp = readMem();
			if ((tmp & 128) == 0)
			{
				continue;
			}
			tmpFCB.dataFlag = tmp;
			for (size_t i = 0; i < 49; i++)
			{
				tmpFCB.name[i] = (char)readMem();
			}
			for (size_t i = 0; i < 6; i++)
			{
				tmpFCB.createTime[i] = (char)readMem();
			}
			for (size_t i = 0; i < 6; i++)
			{
				tmpFCB.changeTime[i] = (char)readMem();
			}
			tmpFCB.storageBlock = readMem();
			tmpFCB.storageBlock *= 256;
			tmpFCB.storageBlock += readMem();
			FCBList.push_back(tmpFCB);
		}
		if (nextBlock == 0)
		{
			break;
		}
	}

}
