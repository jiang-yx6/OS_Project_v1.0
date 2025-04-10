#include "file.h"

File::File()
{
	currentBlock = -1;
	storagePos = 0;
	command = "";//-----------------------------?
	path = "";
	FCBList = new list<MyFCB>;
}
inline char* File::readBlock(int blockNum)
{
	char* block = (char*)malloc(sizeof(BLOCK_SIZE));
	storagePos = blockNum * BLOCK_SIZE;
	for (size_t i = 0; i < BLOCK_SIZE; i++) block[i] =(unsigned char) readStorage();
	return block;
}
inline void File::writeBlock(char* block, int blockNum)
{
	storagePos = blockNum * BLOCK_SIZE;
	for (size_t i = 0; i < BLOCK_SIZE; i++) writeStorage(block[i]);
}
inline int File::newBlock()
{
	int block = -1;
	int flag = 1;
	//如果读取出界会返回-1
	while (flag)
	{
		block++;
		storagePos = block * BLOCK_SIZE + 1;
		flag = readStorage();//为0则为未使用块
		if (flag < 0) break;//即读取失败，创建新块---------读取出界后需重新打开文件
	}
	ioFile.close();
	ioFile.open(FILE_NAME);
	clearBlock(block);

	return block;
}
inline void File::clearBlock(int blockNum)
{
	deleteBlock(blockNum);
	setBlockStage(blockNum, true);
}
inline void File::deleteBlock(int block)
{
	int num = BLOCK_SIZE;
	storagePos = block * BLOCK_SIZE;
	while (num > 0)
	{
		writeStorage(0);
		num--;
	}
}
inline void File::deleteAllNextBlock(int nowBlock)
{
	int nextBlock;

	nextBlock = getNextBlock(nowBlock);
	if (nextBlock != 0)
	{
		setNextBlock(nowBlock, 0);
		while (nextBlock != 0)
		{
			nowBlock = nextBlock;
			setBlockStage(nowBlock, false);
			nextBlock = getNextBlock(nowBlock);
			deleteBlock(nowBlock);
		}
	}
}
inline void File::setBlockStage(int block, bool type)
{
	storagePos = block * BLOCK_SIZE + 1;
	writeStorage(type ? 1 : 0);
}
inline bool File::getBlockStage(int block)
{
	storagePos = block * BLOCK_SIZE + 1;
	return readStorage() ? true : false;
}
inline void File::setNextBlock(int block, int num)
{
	storagePos = block * BLOCK_SIZE + 2;
	writeStorage(num / 256);
	writeStorage(num % 256);
}
inline int File::getNextBlock(int block)
{
	storagePos = block * BLOCK_SIZE + 2;
	return readStorage() * 256 + readStorage();
}

inline void File::writeStorage(unsigned char num)
{
	//直观显示模拟内存中数据
	ioFile.seekp(storagePos * 3);
	int tmp1, tmp2;
	tmp1 = num / 16;
	tmp2 = num % 16;
	num = 2;
	while (num) {
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
	/*ioFile.seekp(storagePos);
	ioFile.put(num);*/

	storagePos++;
}
inline int File::readStorage()
{
	int num = -1;
	//直观显示模拟内存中数据
	ioFile.seekg(storagePos * 3);
	char tmp[1];
	tmp[0] = 0;
	num = 0;
	ioFile.read(tmp, 1);
	while (tmp[0] != ' ') {
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
	/*ioFile.seekg(storagePos);
	num = (unsigned char)ioFile.get();*/

	storagePos++;
	return num;
}

void File::fileControl()
{
	cin.get();//清除主函数中未处理的换行符

	system("cls");
	bool cycleFlag;
	loadMainPath();//要求

	cycleFlag = true;
	cout << "操作方式：\n\
1:cd 路径  路径从~开始为返回主目录，否则从当前目录开始\n\
2:ls 路径  用法同上，但路径可置空，代表查找当前目录\n\
3:mkdir 文件名  在当前目录下创建文件夹\n\
4:rmdir用法同上\n\
注意文件路径使用\\" << endl;

	while (cycleFlag) {
		cout << "[" << path << "]>";
		getline(cin, input);//会正常读取中文字符，处理时可能会出现问题

		inputPos = input.begin();
		findFirstCommand();

		if (!command.empty())
		{
			if (command == "cd")//路径命令，~号代表基地址，使用\分隔文件
			{
				commandChangePath();
				continue;
			}
			if (command == "ls")//后跟文件夹名称或路径或置空
			{
				commandShowPathFile();
				continue;
			}
			if (command == "mkdir")//后跟文件夹名称
			{
				commandCreatePath();
				continue;
			}
			if (command == "rmdir")
			{
				commandDeletePath();
			}
			if (command == "exit")//退出程序
			{
				ioFile.close();
				return;
			}
		}
	}

}
void File::loadMainPath()
{
	//文件命名为storage
	ioFile.open(FILE_NAME);
	if (!ioFile.is_open())//无文件，重新创建
	{
		ioFile.open(FILE_NAME, ios::out);//fstream需已只写模式创建文件
		ioFile.close();
		ioFile.open(FILE_NAME);
		newBlock();//要求
		cout << "文件创建成功" << endl;
		ioFile.close();//保存文件
		ioFile.open(FILE_NAME);
	}

	path = "~";
	currentBlock = 0;
	loadFCB(0);

	//char reader[1024];//seek越界不会提示，如果读取出界则在reader中原有的数据不会改变
}
void File::findFirstCommand()
{
	string tmp = "";
	while (inputPos != input.end() && *inputPos == ' ') inputPos++;//跳过空格
	for (; inputPos != input.end() && *inputPos != ' '; inputPos++) tmp += *inputPos;
	command = tmp;
}
string File::findFileName()
{
	string output;
	int pos;
	pos = command.find('\\');
	if (pos == -1)
	{
		output = command;
		command = "";
	}
	else
	{
		output = command.substr(0, pos);//第二个参数为负数则会读到结尾（无效参数？）
		command = command.substr(pos + 1);
	}
	return output;
}

void File::commandChangePath()
{
	pair<int,int> block;

	findFirstCommand();
	if (command.empty())
	{
		cout << "缺少参数" << endl;
		return;
	}
	block = locateBlock();
	if (block.first == -1)
	{
		cout << "路径不正确" << endl;
		return;
	}
	if (block.second==0)
	{
		cout << "路径不是一个文件夹" << endl;
		return;
	}

	loadFCB(block.first);
	if (strcmp(command.substr(0, command.find('\\')).c_str(), "~")!=0)
	{
		command = path + "\\" + command;
	}
	path = command;
	currentBlock = block.first;
}
void File::commandShowPathFile()
{
	int block;
	MyFCB tmp;

	findFirstCommand();
	block = locateBlock().first;
	if (block==-1)
	{
		cout << "路径不正确" << endl;
	}
	else
	{
		loadFCB(block);
		cout << "-------------------------------------------------------" << endl;
		for (list<MyFCB>::iterator it = FCBList->begin(); it != FCBList->end(); it++)
		{
			tmp = *it;
			cout << "   ";
			cout.width(size(tmp.name));
			cout.setf(ios::left);
			cout << tmp.name;
			cout << "                     ";
			cout << tmp.toTime(0);
			cout << endl;
		}
	}
	loadFCB(currentBlock);
}
void File::commandCreatePath()
{
	MyFCB tmp;
	findFirstCommand();
	if (command.size() > size(tmp.name))
	{
		cout << "文件名长度过长" << endl;
	}
	else if (command.empty()||command == "~" || command.find('\\') != -1)
	{
		cout << "文件名非法" << endl;
	}
	else if (locateBlock() .first!= -1)
	{
		cout << "该文件名已存在" << endl;
	}
	else
	{
		FCBList->push_back(MyFCB(command, 192));

		writeAllFCB();
	}

}
void File::commandDeletePath()
{
	int pos;
	string fileName;
	pair<int, int> block;
	MyFCB* tmpFCB;

	findFirstCommand();
	if (command.empty())
	{
		cout << "缺少参数" << endl;
		return;
	}
	else if (command == path)
	{
		cout << "不允许删除当前目录" << endl;
		return;
	}

	block = locateBlock();
	if (block.first==-1) 
	{
		cout << "路径不正确！" << endl;
		return;
	}
	else if (block.second!=1) 
	{
		cout << "目标不是一个文件目录" << endl;
		return;
	}
	loadFCB(block.first);
	if (FCBList->size()!=0) 
	{
		cout << "要删除的目录不为空" << endl;
		loadFCB(currentBlock);//-----------------return检查
		return;
	}

	deleteAllNextBlock(block.first);
	deleteBlock(block.first);

	pos = command.rfind("\\");
	if (pos==-1)
	{
		fileName = command;
		command = "";
	}
	else
	{
		fileName = command.substr(pos+1);
		command = command.substr(0, pos);//--------------------------输入\开头似乎也正确
	}
	loadFCB(locateBlock().first);
	for (list<MyFCB>::iterator it = FCBList->begin(); it != FCBList->end(); it++)
	{
		tmpFCB = &*it;
		if (strcmp(tmpFCB->name, fileName.c_str()) == 0)//确认文件名称正确
		{
			FCBList->erase(it);
			writeAllFCB();
			break;
		}
	}

	loadFCB(currentBlock);
}

pair<int, int> File::locateBlock()
{
	string fileName, nameCommand;
	int blockNum, type;
	MyFCB* tmpFCB;

	nameCommand = command;//防止命令被破坏

	if (command[0]=='\\')
	{
		//错误输入
		return pair<int, int>(-1, 0);
	}

	type = 1;
	blockNum = currentBlock;
	fileName = findFileName();
	if (fileName == "~")//表明从根目录起
	{
		blockNum = 0;
		fileName = findFileName();
	}
	while (!fileName.empty()) {
		loadFCB(blockNum);
		blockNum = -1;
		for (list<MyFCB>::iterator it = FCBList->begin(); it != FCBList->end(); it++)
		{
			tmpFCB = &*it;
			if (strcmp(tmpFCB->name, fileName.c_str()) == 0)//确认文件名称正确
			{
				blockNum = tmpFCB->storageBlock;
				type = (tmpFCB->dataFlag & 64) ? 1 : 0;
				if (blockNum == 0)//未使用过的文件，为其分配内存
				{
					blockNum = newBlock();
					tmpFCB->storageBlock = blockNum;
					writeAllFCB();
				}
				break;
			}
		}
		fileName = findFileName();
		if (blockNum == -1 || !fileName.empty() && !type)
		{
			fileName = "";
			blockNum = -1;
		}//未查找到对应文件或下一个要打开的目标不为文件夹
	}
	loadFCB(currentBlock);
	command = nameCommand;
	return pair<int, int>(blockNum, type);
}
void File::loadFCB(int blockNum)
{
	FCBList->clear();
	MyFCB tmpFCB;
	int tmp,nextBlock;
	while(1){
		nextBlock = getNextBlock(blockNum);
		for (size_t i = 1; i < BLOCK_SIZE / 64; i++)
		{
			tmp = readStorage();
			if ((tmp & 128) == 0) continue;
			tmpFCB.dataFlag = tmp;
			for (size_t i = 0; i < 49; i++) tmpFCB.name[i] = (char)readStorage();
			for (size_t i = 0; i < 6; i++) tmpFCB.createTime[i] = (unsigned char)readStorage();
			for (size_t i = 0; i < 6; i++) tmpFCB.changeTime[i] = (unsigned char)readStorage();
			tmpFCB.storageBlock = readStorage() * 256 + readStorage();
			FCBList->push_back(tmpFCB);
		}
		if (nextBlock == 0) break;
		blockNum = nextBlock;
	}
}
void File::writeAllFCB()
{
	int nowBlock,nextBlock;

	nowBlock = currentBlock;
	nextBlock = getNextBlock(nowBlock);
	clearBlock(nowBlock);
	storagePos = nowBlock * BLOCK_SIZE + 64;

	for (list<MyFCB>::iterator it = FCBList->begin(); it !=FCBList->end() ; it++)
	{
		if (storagePos==(nowBlock +1)*BLOCK_SIZE)//取巧写法
		{
			//进入下一块/创建新块
			if (nextBlock==0)
			{
				nextBlock = newBlock();
				setNextBlock(nowBlock, nextBlock);
			}
			nowBlock = nextBlock;
			nextBlock = getNextBlock(nowBlock);
			clearBlock(nowBlock);
			storagePos = nowBlock * BLOCK_SIZE + 64;
		}
		writeFCB(it);
	}

	//判断原有块是否均已使用，未使用的删除
	deleteAllNextBlock(nowBlock);
}
void File::writeFCB(list<MyFCB>::iterator it)
{
	MyFCB tmp = *it;
	writeStorage(tmp.dataFlag);
	for (size_t i = 0; i < 49; i++) writeStorage(tmp.name[i]);
	for (size_t i = 0; i < 6; i++) writeStorage(tmp.createTime[i]);
	for (size_t i = 0; i < 6; i++) writeStorage(tmp.changeTime[i]);
	writeStorage(tmp.storageBlock / 256);
	writeStorage(tmp.storageBlock % 256);
}
