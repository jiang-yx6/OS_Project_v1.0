#include "file.h"

File::File()
{
	path = "";
	FCBList = nullptr;
}
//
void File::loginIn()
{
	tryOpenFile();
	cout << "\
ѡ�������\n\
login:��¼\n\
show:��ʾ�����û�\n\
create:�������û�\n\
delete:ɾ���û�\n\
exit:�˳�����\
" << endl;

	string input;
	while (1) {
		cout << "��ѡ�����:";
		cin >> input;
		if (input == "login") { if (commandLogin()) fileControl(); }
		else if (input == "show") commandShowUser();
		else if (input == "create")commandCreateUser();
		else if (input == "delete")commandDeleteUser();
		else if (input == "exit") break;
		else cout << "�밴Ҫ������" << endl;
	}
}

void File::fileControl()
{
	loadMainPath();//Ҫ��
	cin.get();//֮ǰ��δ����Ļ��з��������������ɾ��
	cout << "\
������ʽ��\n\
1:cd ·��  ·����~��ʼΪ������Ŀ¼������ӵ�ǰĿ¼��ʼ\n\
2:ls ·��  �÷�ͬ�ϣ���·�����ÿգ�������ҵ�ǰĿ¼\n\
3:mkdir �ļ���  �ڵ�ǰĿ¼�´����ļ�Ŀ¼\n\
4:rmdir�÷�ͬ��\n\
5:mkfile �ļ��� �ڵ�ǰĿ¼�´����ļ�\n\
6:permission �ļ��� 0~3 �����ļ�Ȩ��\n\
7:logout �˳���¼\n\
ע���ļ�·��ʹ��\\\
" << endl;

	while (1) {
		cout << "[" << userName << "@" << path << "]>";
		getCommand();
		findString(1);

		if (!command.empty())
		{
			if (command == "cd")commandChangePath();
			if (command == "ls")commandShowPathFile();
			if (command == "mkdir")commandCreatePath();
			if (command == "rmdir")commandDeletePath();
			if (command == "mkfile")commandCreateFile();
			if (command == "permission")commandChangePermission();
			if (command == "logout") break;
		}
	}
}
//
void File::commandChangePath()
{
	MyFCBHead* tmp;

	findString(2);
	if (command.empty())
	{
		cout << "ȱ�ٲ���" << endl;
		return;
	}
	tmp = readPathFCBFile(command);
	if (tmp == nullptr)
	{
		cout << "·������ȷ" << endl;
		return;
	}

	if (command[0] != '~') path = path + "\\" + command;
	else path = command;
	FCBList = tmp;
}
void File::commandShowPathFile()
{
	MyFCBHead* tmp;

	findString(2);
	if (command == "")tmp = readPathFCBFile(path);
	else
	{
		tmp = readPathFCBFile(command);
		if (tmp == nullptr) cout << "·������ȷ" << endl;
	}
	cout << "�ļ�����---�ļ�����------------------------------------------��ϵ--�޸�ʱ��---------����ʱ��-------" << endl;
	for (MyFCB* tmpFCB = FCBList->firstFCB; tmpFCB != nullptr; tmpFCB = tmpFCB->next)
	{
		cout << "  ";
		if (tmpFCB->getTypeFlag()) cout << "Ŀ¼";
		else cout << "�ļ�";
		cout << "      ";
		cout.width(NAME_LEN);
		cout.flags(ios::left);
		cout << tmpFCB->getName();
		cout << "";
		if (!(tmpFCB->getTypeFlag()))
		{
			if ((tmpFCB->getOwner()) == userId) cout << "ӵ��";
			else switch ((tmpFCB->getIsReadable() ? 1 : 0) * 2 + (tmpFCB->getIsWritable() ? 1 : 0))
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
		else cout << "    ";
		cout << "  ";
		cout << tmpFCB->getChangeTime();
		cout << "  ";
		cout << tmpFCB->getCreateTime();
		cout << endl;
	}
}
void File::commandCreatePath()
{
	findString(2);
	if (command.size() > 49) cout << "�ļ������ȹ���" << endl;//-----------------------------ʹ�ù̶�ֵ
	else if (command.empty() || command == "~" || command.find('\\') != -1) cout << "�ļ����Ƿ�" << endl;
	else if (FCBList->findFCB(command) != nullptr) cout << "���ļ����Ѵ���" << endl;
	else
	{
		MyFCB* tmp = new MyFCB(true, userId, command);
		tmp->setStorageBlock(findNewBlock());
		setPathParentBlock(tmp->getStorageBlock(), FCBList->getCurrentBlock());
		FCBList->addFCB(tmp);

		writeFCBBlocks(FCBList);
		readFCBBlocks(FCBList->getCurrentBlock());
	}

}
void File::commandCreateFile()
{
	findString(2);
	if (command.size() > 49) cout << "�ļ������ȹ���" << endl;//-----------------------------ʹ�ù̶�ֵ
	else if (command.empty() || command == "~" || command.find('\\') != -1) cout << "�ļ����Ƿ�" << endl;
	else if (FCBList->findFCB(command) != nullptr) cout << "���ļ����Ѵ���" << endl;
	else
	{
		MyFCB* tmp = new MyFCB(false, userId, command);
		tmp->setStorageBlock(findNewBlock());
		//-------------��δȷ���ļ��������и�����λ��
		FCBList->addFCB(tmp);

		writeFCBBlocks(FCBList);
		readFCBBlocks(FCBList->getCurrentBlock());
	}

}
void File::commandDeletePath()
{
	int fileBlock;
	MyFCBHead* tmp;

	findString(2);
	if (command.empty()) cout << "ȱ�ٲ���" << endl;

	else if (command == path) cout << "������ɾ����ǰĿ¼" << endl;
	else if (command == "~") cout << "�����Ƿ�" << endl;
	else
	{
		tmp = readPathFCB(command);
		if (tmp == nullptr)
		{
			cout << "·������ȷ��" << endl;
			return;
		}
		else if (!tmp->firstFCB->getTypeFlag())
		{
			cout << "Ŀ�겻��һ���ļ�Ŀ¼" << endl;
			return;
		}
		tmp = readFCBBlocks(tmp->firstFCB->getStorageBlock());
		if (tmp->firstFCB != nullptr)
		{
			cout << "Ҫɾ����Ŀ¼��Ϊ��" << endl;
			return;
		}

		fileBlock = tmp->getCurrentBlock();
		deleteFCBBlock(fileBlock);
		tmp = readFCBBlocks(tmp->getParentBlock());

		tmp->delFCB(fileBlock);
		writeFCBBlocks(tmp);
	}
}
void File::commandChangePermission()
{
	findString(2);
	MyFCBHead* tmp = readPathFCB(command);
	if (tmp == nullptr)
	{
		cout << "·������ȷ" << endl;
		return;
	}
	tmp = readFCBBlocks(tmp->getParentBlock());
	MyFCB* tmpFCB = tmp->findFCB(command);
	if (tmpFCB->getOwner() != userId)
	{
		cout << "���޸�Ȩ��" << endl;
	}
	else if (tmpFCB->getTypeFlag())
	{
		cout << "����Ϊһ��Ŀ¼" << endl;
	}
	else
	{
		findString(3);
		tmpFCB->setIsReadable(input[0] - '0');
		tmpFCB->setIsWritable(input[1] - '0');
		writeFCBBlocks(tmp);
	}
}
void File::loadMainPath()
{
	path = "~";
	FCBList = readPathFCBFile(path);
	//char reader[1024];//seekԽ�粻����ʾ�������ȡ��������reader��ԭ�е����ݲ���ı�
}
MyFCBHead* File::readPathFCB(string filePath)
{
	string fileName;
	MyFCBHead* tmp = FCBList;
	MyFCB* tmpFCB = nullptr;

	if (filePath == "~" || filePath == "" || filePath[0] == '\\') return nullptr;
	fileName = findFirstFileName(&filePath);
	if (fileName == "~")//�����Ӹ�Ŀ¼��
	{
		tmp = readFCBBlocks(1);
		fileName = findFirstFileName(&filePath);
	}
	while (1) {
		if (fileName=="..")//���븸Ŀ¼
		{
			if (tmp->getCurrentBlock() == 1) return nullptr;
			tmp = readFCBBlocks(tmp->getParentBlock());
			fileName = findFirstFileName(&filePath);
			if (fileName.empty())
			{
				if (tmp->getCurrentBlock() == 1) break;
				int tmpInt = tmp->getCurrentBlock();
				tmp = readFCBBlocks(tmp->getParentBlock());
				tmp->firstFCB = tmp->findFCB(tmpInt);
				return tmp;
			}
		}
		else
		{
			tmpFCB = tmp->findFCB(fileName);
			if (tmpFCB == nullptr || !tmpFCB->getCreateFlag()) return nullptr;
			fileName = findFirstFileName(&filePath);
			if (fileName.empty()) break;
			if (!tmpFCB->getTypeFlag() || tmpFCB->getStorageBlock() == 0) return nullptr;//��һ��Ҫ�򿪵�Ŀ�겻Ϊ�ļ���
			tmp = readFCBBlocks(tmpFCB->getStorageBlock());
		}
	}
	tmp->firstFCB = tmpFCB;
	return tmp;
}
MyFCBHead* File::readPathFCBFile(string filePath)
{
	if (filePath=="~") return readFCBBlocks(1);
	MyFCBHead* tmp = readPathFCB(filePath);
	if (tmp == nullptr)return nullptr;
	return readFCBBlocks(tmp->firstFCB->getStorageBlock());
}
string File::findFirstFileName(string* input)
{
	string output;
	int pos;
	pos = input->find('\\');
	if (pos == -1)
	{
		output = *input;
		*input = "";
	}
	else
	{
		output = input->substr(0, pos);//�ڶ�������Ϊ������������β����Ч��������
		*input = input->substr(pos + 1);
	}
	return output;
}
void File::writeFCBBlocks(MyFCBHead* input)
{
	int nowBlock, nextBlock, sumBlock;
	nowBlock = input->getCurrentBlock();
	nextBlock = getNextFCBBlock(nowBlock);
	sumBlock = getSumBlockNum();
	clearBlock(nowBlock);
	setBlockStage(nowBlock, true);
	if (input->getCurrentBlock()!=1) setPathParentBlock(nowBlock, input->getParentBlock());
	else setPathParentBlock(nowBlock, sumBlock);

	if (input->firstFCB != nullptr)
	{
		int i = 1;
		for (MyFCB* tmp = input->firstFCB; tmp != nullptr; tmp = tmp->next)
		{
			if (!tmp->getCreateFlag()) continue;
			if (i == BLOCK_SIZE / 64)
			{
				//������һ��/�����¿�
				if (nextBlock == 0)
				{
					nextBlock = findNewBlock();
					setNextFCBBlock(nowBlock, nextBlock);
				}
				nowBlock = nextBlock;
				nextBlock = getNextFCBBlock(nowBlock);
				clearBlock(nowBlock);
				setBlockStage(nowBlock, true);
				i = 1;
			}
			writeFCB(nowBlock, i, tmp);
			i++;
			if (tmp==nullptr)
			{
				MyFCB* tmpFCB = new MyFCB(true, 0, "");
				tmpFCB->deleteFCB();
				for (; i < BLOCK_SIZE / 64; i++) writeFCB(nowBlock, i, tmpFCB);
			}
		}
	}

	//�ж�ԭ�п��Ƿ����ʹ�ã�δʹ�õ�ɾ��
	setNextFCBBlock(nowBlock, 0);
	if (nextBlock != 0) deleteFCBBlock(nextBlock);
}
MyFCBHead* File::readFCBBlocks(int blockNum)
{
	MyFCBHead* head = new MyFCBHead(blockNum);
	head->setParentBlock(getPathParentBlock(blockNum));
	MyFCB* tmp;
	int nowBlock, nextBlock;
	nowBlock = blockNum;
	do{
		nextBlock = getNextFCBBlock(nowBlock);
		for (size_t i = 1; i < BLOCK_SIZE / 64; i++)
		{
			tmp = new MyFCB(readLine(nowBlock, i * 64, 64));
			if (tmp->getCreateFlag()) head->addFCB(tmp);
		}
		nowBlock = nextBlock;
	} while (nowBlock != 0);
	return head;
}

bool File::commandLogin()
{
	string name,password;
	User* user;
	cout << "�������û�����";
	cin >> name;
	user = checkUserName(name);
	if (user == nullptr)
	{
		cout << "�û�������" << endl;
		return false;
	}
	cout << "����������:";
	cin >> password;
	if (!user->cmpPassword(password))
	{
		cout << "�������" << endl;
		return false;
	}
	userId = user->getId();
	userName = user->getName();
	cout << "��¼�ɹ�" << endl;
	return true;
}
void File::commandShowUser()
{
	User* user;
	cout << "--id----�û�����----------------------------------------------------------" << endl;
	for (size_t i=0; i < 16; i++)
	{
		user = readUser(i);
		if (!user->getEnableFlag()) continue;
		cout << "  ";
		cout.width(2);
		cout.setf(ios::right);
		cout << user->getId();
		cout << "    ";
		cout.width(size(user->getName()));
		cout.setf(ios::left);
		cout << user->getName();
		cout << endl;
	}
}
void File::commandCreateUser()
{
	User* user = getNewUser();
	string name, password;
	if (user == nullptr)
	{
		cout << "�û������Ѵ�����" << endl;
		return;
	}
	cout << "�������û�����";
	cin >> name;
	if (checkUserName(name) != nullptr)
	{
		cout << "�û����Ѵ���" << endl;
		return;
	}
	cout << "����������:";
	cin >> password;
	writeUser(new User(user->getId(), name, password));
}
void File::commandDeleteUser()
{
	User* user;
	string name, password;
	cout << "�������û�����";
	cin >> name;
	user = checkUserName(name);
	if (user == nullptr)
	{
		cout << "�û�������" << endl;
		return;
	}
	cout << "����������:";
	cin >> password;
	if (!user->cmpPassword(password))
	{
		cout << "�������" << endl;
		return;
	}
	deleteUser(user->getId());
	cout << "ɾ���ɹ�" << endl;
}
User* File::checkUserName(string input)
{
	User* user;
	for (size_t i = 0; i < 16; i++)
	{
		user = readUser(i);
		if (user->getId() != -1) if (user->cmpName(input)) return user;
	}
	return nullptr;
}
User* File::getNewUser()
{
	User* user;
	for (size_t i = 0; i < 16; i++)
	{
		user = readUser(i);
		if (!user->getEnableFlag()) return user;
	}
	return nullptr;
}
//
void File::findString(int num)
{
	string::iterator pos = input.begin();
	for (size_t i=0; i < num; i++)
	{
		command = "";
		while (pos != input.end() && *pos == ' ') pos++;//�����ո�
		for (; pos != input.end() && *pos != ' '; pos++) command += *pos;
	}

}

inline void File::writeFCB(int blockNum, int FCBNum, MyFCB* input)
{
	writeLine(input->toString(), blockNum, FCBNum * 64, 64);
}
inline MyFCB* File::readFCB(int blockNum, int FCBNum)
{
	return new MyFCB(readLine(blockNum, FCBNum * 64, 64));
}
void File::deleteFCBBlock(int nowBlock)
{
	int nextBlock;
	nextBlock = getNextFCBBlock(nowBlock);
	clearBlock(nowBlock);
	while (nextBlock != 0)
	{
		nowBlock = nextBlock;
		nextBlock = getNextFCBBlock(nowBlock);
		setBlockStage(nowBlock, false);
		clearBlock(nowBlock);
	}
}
inline void File::setNextFCBBlock(int blockNum, int num)
{
	char input[2] = { num % 256,num / 256 };
	writeLine(input, blockNum, 2, 2);
}
inline int File::getNextFCBBlock(int blockNum)
{
	unsigned char* output;
	output = (unsigned char*)readLine(blockNum, 2, 2);
	return output[1] * 256 + output[0];
}
inline void File::setPathParentBlock(int blockNum, int parentNum)
{
	char input[2] = { parentNum % 256,parentNum / 256 };
	writeLine(input, blockNum, 4, 2);
}
inline int File::getPathParentBlock(int blockNum)
{
	char* output;
	output = readLine(blockNum, 4, 2);
	return output[1] * 256 + output[0];
}

void File::deleteUser(int id)
{
	User* tmp=new User(id);
	tmp->disableUser();
	writeUser(tmp);
}
inline User* File::readUser(int id)
{
	User* output = new User(readLine(0, id * 64, 64));
	output->setId(id);
	return output;
}
inline void File::writeUser(User* input)
{
	writeLine(input->toString(), 0, input->getId() * 64, 64);
}
//
inline void File::getCommand()
{
	getline(cin, input);//��������ȡ�����ַ�������ʱ���ܻ��������
}

inline int File::findNewBlock()
{
	int sumBlockNum = getSumBlockNum();
	for (size_t i = 0; i < sumBlockNum; i++)
	{
		if (!getBlockStage(i))
		{
			clearBlock(i);
			setBlockStage(i, true);
			return i;
		}
	}
	int blockNum = createBlock();
	clearBlock(blockNum);
	setBlockStage(blockNum, true);
	return blockNum;
}
inline void File::writeBlock(char* input, int blockNum)
{
	writeLine(input, blockNum, 0, BLOCK_SIZE);
}
inline char* File::readBlock(int blockNum)
{
	char* output;
	output = readLine(blockNum, 0, BLOCK_SIZE);
	return output;
}
inline void File::clearBlock(int blockNum)
{
	for (size_t i = 0; i < BLOCK_SIZE; i++) writeStorage(0, blockNum, i);
}
inline void File::writeLine(char* input, int blockNum, int posNum, int len)
{
	for (size_t i = 0; i < len; i++) writeStorage(input[i], blockNum, posNum + i);
}
inline char* File::readLine(int blockNum, int posNum, int len)
{
	char* output = new char[len];
	for (size_t i = 0; i < len; i++) output[i] = readStorage(blockNum, posNum + i);
	return output;
}
inline int File::getSumBlockNum()
{
	unsigned char* output;
	output = (unsigned char*)readLine(1, 4, 2);
	return output[0] + output[1] * 256;
}
inline void File::setSumBlockNum(int sumBlockNum)
{
	char input[2] = { sumBlockNum % 256,sumBlockNum / 256 };
	writeLine(input, 1, 4, 2);
}
inline void File::setBlockStage(int blockNum, bool status)
{
	writeStorage(status ? 1 : 0, blockNum, 1);
}
inline bool File::getBlockStage(int blockNum)
{
	return readStorage(blockNum,1) ? true : false;
}
//
string File::charToString(char* input)
{
	string output = "";
	for (size_t i = 0; i < sizeof(input); i++)
	{
		if (input[i] == '\0')break;
		output += input[i];
	}
	return output;
}
void File::tryOpenFile()
{
	ioFile.open(FILE_NAME);
	if (!ioFile.is_open())//���ļ������´���
	{
		ioFile.open(FILE_NAME, ios::out);//fstream����ֻдģʽ�����ļ�
		ioFile.close();
		ioFile.open(FILE_NAME);
		//���������û���idΪ0������Ϊroot������Ϊ123456
		for (size_t i = 0; i < BLOCK_SIZE; i++) writeStorage(0, 0, i);
		for (size_t i = 0; i < BLOCK_SIZE; i++) writeStorage(0, 1, i);
		setBlockStage(1, true);
		setSumBlockNum(2);
		User* user=new User(0, "root", "123456");
		writeUser(user);
		cout << "�ļ������ɹ�" << endl;
	}
}
inline int File::createBlock()
{
	clearBlock(getSumBlockNum());
	setSumBlockNum(getSumBlockNum()+1);
	return getSumBlockNum()-1;
}
inline void File::writeStorage(unsigned char input, int blockNum, int posNum)
{
	//ֱ����ʾģ���ڴ�������
	ioFile.seekp((blockNum * BLOCK_SIZE + posNum) * 3);
	switch (input / 16)
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
		ioFile.write(to_string(input / 16).c_str(), 1);
		break;
	}
	switch (input % 16)
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
		ioFile.write(to_string(input % 16).c_str(), 1);
		break;
	}
	ioFile.write(" ", 1);

	//ʹ��16����ascii��鿴�����鿴�ڴ�����
	/*ioFile.seekp(blockNum * BLOCK_SIZE + posNum);
	ioFile.put(input);*/
}
inline unsigned char File::readStorage(int blockNum, int posNum)
{
	int output = 0;
	char tmp[2];
	//ֱ����ʾģ���ڴ�������
	ioFile.seekg((blockNum * BLOCK_SIZE + posNum) * 3);
	ioFile.read(tmp, 2);
	switch (tmp[0])
	{
	case 'A':
		output += 10;
		break;
	case 'B':
		output += 11;
		break;
	case 'C':
		output += 12;
		break;
	case 'D':
		output += 13;
		break;
	case 'E':
		output += 14;
		break;
	case 'F':
		output += 15;
		break;
	default:
		output += (unsigned char)tmp[0] - 48;
		break;
	}
	output *= 16;
	switch (tmp[1])
	{
	case 'A':
		output += 10;
		break;
	case 'B':
		output += 11;
		break;
	case 'C':
		output += 12;
		break;
	case 'D':
		output += 13;
		break;
	case 'E':
		output += 14;
		break;
	case 'F':
		output += 15;
		break;
	default:
		output += (unsigned char)tmp[1] - 48;
		break;
	}
	//ʹ��16����ascii��鿴�����鿴�ڴ�����
	/*ioFile.seekg(blockNum * BLOCK_SIZE + posNum);
	output = (unsigned char)ioFile.get();*/

	return output;
}