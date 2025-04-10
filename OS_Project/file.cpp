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
	//�����ȡ����᷵��-1
	while (flag)
	{
		block++;
		storagePos = block * BLOCK_SIZE + 1;
		flag = readStorage();//Ϊ0��Ϊδʹ�ÿ�
		if (flag < 0) break;//����ȡʧ�ܣ������¿�---------��ȡ����������´��ļ�
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
	//ֱ����ʾģ���ڴ�������
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

	//ʹ��16����ascii��鿴�����鿴�ڴ�����
	/*ioFile.seekp(storagePos);
	ioFile.put(num);*/

	storagePos++;
}
inline int File::readStorage()
{
	int num = -1;
	//ֱ����ʾģ���ڴ�������
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
	//ʹ��16����ascii��鿴�����鿴�ڴ�����
	/*ioFile.seekg(storagePos);
	num = (unsigned char)ioFile.get();*/

	storagePos++;
	return num;
}

void File::fileControl()
{
	cin.get();//�����������δ����Ļ��з�

	system("cls");
	bool cycleFlag;
	loadMainPath();//Ҫ��

	cycleFlag = true;
	cout << "������ʽ��\n\
1:cd ·��  ·����~��ʼΪ������Ŀ¼������ӵ�ǰĿ¼��ʼ\n\
2:ls ·��  �÷�ͬ�ϣ���·�����ÿգ�������ҵ�ǰĿ¼\n\
3:mkdir �ļ���  �ڵ�ǰĿ¼�´����ļ���\n\
4:rmdir�÷�ͬ��\n\
ע���ļ�·��ʹ��\\" << endl;

	while (cycleFlag) {
		cout << "[" << path << "]>";
		getline(cin, input);//��������ȡ�����ַ�������ʱ���ܻ��������

		inputPos = input.begin();
		findFirstCommand();

		if (!command.empty())
		{
			if (command == "cd")//·�����~�Ŵ������ַ��ʹ��\�ָ��ļ�
			{
				commandChangePath();
				continue;
			}
			if (command == "ls")//����ļ������ƻ�·�����ÿ�
			{
				commandShowPathFile();
				continue;
			}
			if (command == "mkdir")//����ļ�������
			{
				commandCreatePath();
				continue;
			}
			if (command == "rmdir")
			{
				commandDeletePath();
			}
			if (command == "exit")//�˳�����
			{
				ioFile.close();
				return;
			}
		}
	}

}
void File::loadMainPath()
{
	//�ļ�����Ϊstorage
	ioFile.open(FILE_NAME);
	if (!ioFile.is_open())//���ļ������´���
	{
		ioFile.open(FILE_NAME, ios::out);//fstream����ֻдģʽ�����ļ�
		ioFile.close();
		ioFile.open(FILE_NAME);
		newBlock();//Ҫ��
		cout << "�ļ������ɹ�" << endl;
		ioFile.close();//�����ļ�
		ioFile.open(FILE_NAME);
	}

	path = "~";
	currentBlock = 0;
	loadFCB(0);

	//char reader[1024];//seekԽ�粻����ʾ�������ȡ��������reader��ԭ�е����ݲ���ı�
}
void File::findFirstCommand()
{
	string tmp = "";
	while (inputPos != input.end() && *inputPos == ' ') inputPos++;//�����ո�
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
		output = command.substr(0, pos);//�ڶ�������Ϊ������������β����Ч��������
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
		cout << "ȱ�ٲ���" << endl;
		return;
	}
	block = locateBlock();
	if (block.first == -1)
	{
		cout << "·������ȷ" << endl;
		return;
	}
	if (block.second==0)
	{
		cout << "·������һ���ļ���" << endl;
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
		cout << "·������ȷ" << endl;
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
		cout << "�ļ������ȹ���" << endl;
	}
	else if (command.empty()||command == "~" || command.find('\\') != -1)
	{
		cout << "�ļ����Ƿ�" << endl;
	}
	else if (locateBlock() .first!= -1)
	{
		cout << "���ļ����Ѵ���" << endl;
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
		cout << "ȱ�ٲ���" << endl;
		return;
	}
	else if (command == path)
	{
		cout << "������ɾ����ǰĿ¼" << endl;
		return;
	}

	block = locateBlock();
	if (block.first==-1) 
	{
		cout << "·������ȷ��" << endl;
		return;
	}
	else if (block.second!=1) 
	{
		cout << "Ŀ�겻��һ���ļ�Ŀ¼" << endl;
		return;
	}
	loadFCB(block.first);
	if (FCBList->size()!=0) 
	{
		cout << "Ҫɾ����Ŀ¼��Ϊ��" << endl;
		loadFCB(currentBlock);//-----------------return���
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
		command = command.substr(0, pos);//--------------------------����\��ͷ�ƺ�Ҳ��ȷ
	}
	loadFCB(locateBlock().first);
	for (list<MyFCB>::iterator it = FCBList->begin(); it != FCBList->end(); it++)
	{
		tmpFCB = &*it;
		if (strcmp(tmpFCB->name, fileName.c_str()) == 0)//ȷ���ļ�������ȷ
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

	nameCommand = command;//��ֹ����ƻ�

	if (command[0]=='\\')
	{
		//��������
		return pair<int, int>(-1, 0);
	}

	type = 1;
	blockNum = currentBlock;
	fileName = findFileName();
	if (fileName == "~")//�����Ӹ�Ŀ¼��
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
			if (strcmp(tmpFCB->name, fileName.c_str()) == 0)//ȷ���ļ�������ȷ
			{
				blockNum = tmpFCB->storageBlock;
				type = (tmpFCB->dataFlag & 64) ? 1 : 0;
				if (blockNum == 0)//δʹ�ù����ļ���Ϊ������ڴ�
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
		}//δ���ҵ���Ӧ�ļ�����һ��Ҫ�򿪵�Ŀ�겻Ϊ�ļ���
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
		if (storagePos==(nowBlock +1)*BLOCK_SIZE)//ȡ��д��
		{
			//������һ��/�����¿�
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

	//�ж�ԭ�п��Ƿ����ʹ�ã�δʹ�õ�ɾ��
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
