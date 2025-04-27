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
	int block = 0;
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
	system("cls");
	loginIn();//��¼
	loadMainPath();//Ҫ��
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
		cout << "[" << userName << "@" << path << "]>";
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
			if (command == "mkfile")
			{
				commandCreateFile();
			}
			if (command == "permission")
			{
				commandChangePermission();
			}
			if (command == "logout")
			{
				system("cls");
				ioFile.close();
				loginIn();//��¼
				loadMainPath();//Ҫ��
				cin.get();//�����������δ����Ļ��з�
				system("cls");
			}
			if (command == "exit")//�˳�����
			{
				ioFile.close();
				return;
			}
		}
	}

}

void File::loginIn()
{
	//�ļ�����Ϊstorage
	ioFile.open(FILE_NAME);
	if (!ioFile.is_open())//���ļ������´���
	{
		ioFile.open(FILE_NAME, ios::out);//fstream����ֻдģʽ�����ļ�
		ioFile.close();
		ioFile.open(FILE_NAME);
		newNameBlock();
		cout << "�ļ������ɹ�" << endl;
		ioFile.close();//�����ļ�
		ioFile.open(FILE_NAME);
	}



	cout << "ѡ�������\n\
1:��¼\n\
2:�������û�\n\
3:ɾ���û�" << endl;
	int input=0,id;
	string name, password;
	User user;
	bool endFlag = false;

	while(!endFlag){
		cout << "��ѡ�����:";
		cin >> input;
		switch (input)
		{
		case 1:
			cout << "�������û�����";
			cin >> name;
			id = checkUserName(name);
			if (id == -1) 
			{
				cout << "�û�������" << endl;
				break;
			}
			cout << "����������:";
			cin >> password;
			user = User(readUser(id));
			if (user.cmpPassword(password))
			{
				userId = user.getId();
				userName = user.name;
				cout << "��¼�ɹ�" << endl;
				endFlag = true;
				break;
			}
			cout << "�������" << endl;
			break;
		case 2:
			id = getNewId();
			if (id==-1)
			{
				cout << "�û������Ѵ�����" << endl;
				break;
			}
			cout << "�������û�����";
			cin >> name;
			if (checkUserName(name) != -1)
			{
				cout << "�û����Ѵ���" << endl;
				break;
			}
			cout << "����������:";
			cin >> password;
			writeUser(User(id, name, password));
			break;
		case 3:
			cout << "�������û�����";
			cin >> name;
			id = checkUserName(name);
			if (id == -1)
			{
				cout << "�û�������" << endl;
				break;
			}
			cout << "����������:";
			cin >> password;
			user = User(readUser(id));
			if (user.cmpPassword(password))
			{
				deleteUser(id);
				cout << "ɾ���ɹ�" << endl;
				break;
			}
			cout << "�������" << endl;
			break;
		default:
			cout << "�밴Ҫ������" << endl;
			break;
		}
	}
}
int File::getNewId()
{
	User user;
	for (size_t i = 0; i < 16; i++)
	{
		user = User(readUser(i));
		if (user.getId() == 255) return i;
	}
	return -1;
}
int File::checkUserName(string name)
{
	User user;
	for (size_t i = 0; i < 16; i++)
	{
		user = User(readUser(i));
		if (user.getId() != -1) if (user.cmpName(name))
		{
			return user.getId();
		}
	}
	return -1;
}
void File::newNameBlock()
{
	//���������û���idΪ0������Ϊroot������Ϊ123456
	clearBlock(0);
	clearBlock(1);
	User user(0, "root", "123456");
	writeUser(user);
}
char* File::readUser(int id)
{
	storagePos = id * 64;
	char* input = new char[64];
	for (size_t i = 0; i < 64; i++) input[i] = readStorage();
	return input;
}
void File::writeUser(User input)
{
	storagePos = input.getId()*64;
	char* output = input.toString();
	for (size_t i = 0; i < 64; i++) writeStorage(output[i]);
}
void File::deleteUser(int id)
{
	storagePos = id * 64;
	for (size_t i = 0; i < 64; i++) writeStorage(0);
}
void File::loadMainPath()
{
	path = "~";
	currentBlock = 1;
	loadFCB(1);
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
		cout << "�ļ�����---�ļ�����------------------------------------------��ϵ--�޸�ʱ��---------����ʱ��-------" << endl;
		for (list<MyFCB>::iterator it = FCBList->begin(); it != FCBList->end(); it++)
		{
			tmp = *it;
			cout << "  ";
			if (tmp.dataFlag&0b01000000) cout << "Ŀ¼";
			else cout << "�ļ�";
			cout << "      ";
			cout.width(size(tmp.name));
			cout.setf(ios::left);
			cout << tmp.name;
			cout << "";
			if (!(tmp.dataFlag & 0b01000000))
			{
				if ((tmp.dataFlag & 0b00001111) == userId) cout << "ӵ��";
				else switch ((tmp.dataFlag / 16)&0b0011)
				{
				case 0:
					cout << "����";
					break;
				case 1:
					cout << "ֻд";
					break;
				case 2:
					cout << "ֻ��";
					break;
				case 3:
					cout << "����";
					break;
				}
			}
			else
			{
				cout << "    ";
			}
			cout << "  ";
			cout << tmp.toTime(0);
			cout << "  ";
			cout << tmp.toTime(1);
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
		FCBList->push_back(MyFCB(command, 0b11000000));

		writeAllFCB();
	}

}
void File::commandCreateFile()
{
	MyFCB tmp;
	findFirstCommand();
	if (command.size() > size(tmp.name))
	{
		cout << "�ļ������ȹ���" << endl;
	}
	else if (command.empty() || command == "~" || command.find('\\') != -1)
	{
		cout << "�ļ����Ƿ�" << endl;
	}
	else if (locateBlock().first != -1)
	{
		cout << "���ļ����Ѵ���" << endl;
	}
	else
	{
		FCBList->push_back(MyFCB(command, 0b10000000+userId));

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
void File::commandChangePermission()
{
	findFirstCommand();
	MyFCB* tmp = locateNowFCB();
	if (tmp==NULL)
	{
		cout << "�ļ�������" << endl;
	}
	else if ((tmp->dataFlag&0b00001111)!=userId)
	{
		cout << "���޸�Ȩ��" << endl;
	}
	else if (tmp->dataFlag&0b01000000)
	{
		cout << "����Ϊһ��Ŀ¼" << endl;
	}
	else
	{
		findFirstCommand();
		tmp->dataFlag = (tmp->dataFlag & 0b11001111) + ((command[0] - '0') << 4);
		writeAllFCB();
	}
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
		blockNum = 1;
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
void File::changeFCB(int type, int property, string name)
{

}
MyFCB* File::locateNowFCB()
{
	string fileName, nameCommand;
	MyFCB* tmpFCB;

	nameCommand = command;//��ֹ����ƻ�

	fileName = findFileName();
	for (list<MyFCB>::iterator it = FCBList->begin(); it != FCBList->end(); it++)
	{
		tmpFCB = &*it;
		if (strcmp(tmpFCB->name, fileName.c_str()) == 0)return &*it;//ȷ���ļ�������ȷ
	}
	command = nameCommand;
	return NULL;
}
