#include "file.h"

//#define DEBUG

File::File()
{
	currentBlock = 0;
	memPos = 0;
	command = "";
	path = "~";
	FCBList = new list<MyFCB>;
}

void File::fileControl()
{
	system("cls");

	bool cycleFlag = true;
	cin.get();//清除主函数中未处理的换行符
	loadMainPath();

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
			if (command=="ls")//后跟文件夹名称或路径或置空
			{
				commandShowPathFile();
				continue;
			}
			if (command=="mkdir")//后跟文件夹名称
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

void File::loadMainPath()
{
	//文件命名为storage
	ioFile.open("storage.txt");
	if (!ioFile.is_open())//无文件，重新创建
	{
		ioFile.open("storage.txt",ios::out);//fstream需已只写模式创建文件
		ioFile.close();
		ioFile.open("storage.txt");
		newBlock();
		cout << "文件创建成功" << endl;
		ioFile.close();//保存文件
		ioFile.open("storage.txt");
	}

	command = "~";
	loadFCB(locateBlock());

	//char reader[1024];//seek越界不会提示，如果读取出界则在reader中原有的数据不会改变
}

void File::commandChangePath()
{
	/*command = "";
	findFirstCommand();
	locateBlock();*/
}

void File::commandShowPathFile()
{
	int block;
	MyFCB tmp;
	findFirstCommand();
	block = locateBlock();
	if (block==-1)
	{
		cout << "路径不正确" << endl;
	}
	else if(block||command=="~"||command.empty() && path == "~")
	{
		loadFCB(block);
		cout << "-------------------------------------------------------" << endl;
		for (list<MyFCB>::iterator it = FCBList->begin(); it != FCBList->end(); it++)
		{
			tmp = *it;
			cout << "   ";
			cout.width(49);
			cout.setf(ios::left);
			cout << tmp.name;
			cout << "                     ";
			cout << tmp.toTime(0);
			cout << endl;
		}
	}
	else
	{
		cout << "-------------------------------------------------------" << endl;
	}
	
}

void File::commandCreatePath()
{
	MyFCB tmp;
	command = "";
	findFirstCommand();
	if (command.size() > 49)
	{
		cout << "文件名长度过长" << endl;
	}
	else if (command.empty()||command == "~" || command.find('\\') != -1)
	{
		cout << "文件名非法" << endl;
	}
	else if (locateBlock() != -1)
	{
		cout << "该文件名已存在" << endl;
	}
	/*for (list<MyFCB>::const_iterator it=FCBList->begin(); it != FCBList->end(); it++)
	{
		tmp=*it;
		if (strcmp(tmp.name , command.c_str())==0)
		{
			cout << "该文件名已存在" << endl;
			return;
		}
	}*///可用locatepath优化？
	else
	{
		FCBList->push_back(MyFCB(command, 192));

		writeAllFCB();
	}

}

//void File::fileInitialize()
//{
//	addBlock(0);
//
//	memPos = 1;
//	writeMem(1);//标记0号块已使用
//}

void File::clearBlock(int blockNum)
{
	memPos = BLOCK_SIZE * blockNum;
	placeHold(BLOCK_SIZE);
	memPos = blockNum * BLOCK_SIZE+1;
	writeMem(1);
}

void File::placeHold(int num)
{
	while (num > 0)
	{
		writeMem(0);
		num--;
	}
}

void File::writeMem(char ch)
{
	//0~255
	//直观显示模拟内存中数据
	unsigned char num = ch;
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
	tmp[0] = 0;
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
		case 0:
			return -1;
		default:
			num += (int)tmp[0] - 48;
			break;
		}
		tmp[0] = 0;
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
	string tmpCommand=command + '\\';
	size_t cutPos = 0, cutEnd;
	int blockNum = currentBlock;
	list<MyFCB>* tmp=nullptr;
	string fileName="";
	MyFCB tmpFCB;

	tmp = FCBList;
	FCBList = new list<MyFCB>;

	cutEnd = tmpCommand.find('\\', cutPos);
	fileName = tmpCommand.substr(cutPos,cutEnd);//第二个参数为-1则会读到结尾（无效参数？）
	cutPos = cutEnd+1;
	if (fileName=="~")
	{
		//表明从根目录起
		blockNum = 0;
		cutEnd = tmpCommand.find('\\', cutPos);
		fileName = tmpCommand.substr(cutPos,cutEnd);
		cutPos = cutEnd + 1;
	}

	while(!fileName.empty()){
		loadFCB(blockNum);
		blockNum = -1;
		for (list<MyFCB>::iterator it = FCBList->begin(); it != FCBList->end(); it++)
		{
			tmpFCB = *it;
			if (strcmp(tmpFCB.name,fileName.c_str())==0)//确认文件名称正确
			{
				blockNum = tmpFCB.storageBlock;
				break;
			}
		}
		if (blockNum==-1)
		{
			return blockNum;//未查找到对应文件
		}
		if (cutEnd==-1)
		{
			break;
		}
		cutEnd = tmpCommand.find('\\', cutPos);
		fileName = tmpCommand.substr(cutPos, cutEnd);
		cutPos = cutEnd + 1;
		if (!((unsigned char)tmpFCB.dataFlag & 64))//目标不为文件夹
		{
			if (!fileName.empty())
			{
				//cout << "目标不是一个文件夹" << endl;
				return -1;
			}
		}
	}

	FCBList->clear();
	FCBList = tmp;

	return blockNum;
}

void File::loadFCB(int blockNum)
{
	currentBlock = blockNum;
	FCBList->clear();
	MyFCB tmpFCB;
	int tmp,nextBlock;
	while(1){
		memPos = blockNum * BLOCK_SIZE + 2;
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
			FCBList->push_back(tmpFCB);
		}
		if (nextBlock == 0)
		{
			break;
		}
		blockNum = nextBlock;
	}

}

void File::writeAllFCB()
{
	int block,nextBlock;
	command = "";
	block=locateBlock();
	memPos = block * BLOCK_SIZE+2;
	nextBlock = readMem() * 256 + readMem();

	clearBlock(block);
	memPos = block * BLOCK_SIZE + 64;
	for (list<MyFCB>::iterator it = FCBList->begin(); it !=FCBList->end() ; it++)
	{
		if (memPos==(block+1)*BLOCK_SIZE)//取巧写法
		{
			//进入下一块/创建新块
			if (nextBlock!=0)
			{
				block = nextBlock;
				memPos = block * BLOCK_SIZE + 2;
				nextBlock = readMem() * 256 + readMem();
				clearBlock(block);
				memPos = block * BLOCK_SIZE + 64;
			}
			else
			{
				nextBlock = newBlock();
				memPos = block * BLOCK_SIZE + 2;
				writeMem(nextBlock / 256);
				writeMem(nextBlock % 256);
				block = nextBlock;
				nextBlock = 0;
				memPos = block * BLOCK_SIZE + 64;
			}
		}

		writeFCB(it);
	}

	//判断原有块是否均已使用，未使用的删除
	if (nextBlock!=0)
	{
		memPos = block * BLOCK_SIZE + 2;
		writeMem(0);
		writeMem(0);
		while (nextBlock != 0)
		{
			block = nextBlock;
			memPos = block * BLOCK_SIZE;
			writeMem(0);
			writeMem(0);
			memPos = block * BLOCK_SIZE + 2;
			nextBlock = readMem() * 256 + readMem();
		}
	}

}

int File::newBlock()
{
	int block = -1;
	int flag=1;
	//如果读取出界会返回-1
	while (flag)
	{
		block++;
		memPos = block * BLOCK_SIZE;
		flag = readMem() * 256 + readMem();
		if (flag<0) break;//即读取失败，创建新块---------读取出界后需重新打开文件
	}
	ioFile.close();
	ioFile.open("storage.txt");
	clearBlock(block);

	return block;
}

void File::writeFCB(list<MyFCB>::iterator it)
{
	MyFCB tmp = *it;
	writeMem(tmp.dataFlag);
	for (size_t i = 0; i < 49; i++) writeMem(tmp.name[i]);
	for (size_t i = 0; i < 6; i++) writeMem(tmp.createTime[i]);
	for (size_t i = 0; i < 6; i++) writeMem(tmp.changeTime[i]);
	writeMem(tmp.storageBlock / 256);
	writeMem(tmp.storageBlock % 256);
}
