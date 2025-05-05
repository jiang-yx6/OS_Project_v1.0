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
选择操作：\n\
login:登录\n\
show:显示所有用户\n\
create:创建新用户\n\
delete:删除用户\n\
exit:退出程序\
" << endl;

	string input;
	while (1) {
		cout << "请选择操作:";
		cin >> input;
		if (input == "login") { if (commandLogin()) fileControl(); }
		else if (input == "show") commandShowUser();
		else if (input == "create")commandCreateUser();
		else if (input == "delete")commandDeleteUser();
		else if (input == "exit") break;
		else cout << "请按要求输入" << endl;
	}
}

void File::fileControl()
{
	loadMainPath();//要求
	cin.get();//之前中未处理的换行符，若处理后则需删除
	cout << "\
操作方式：\n\
1:cd 路径  路径从~开始为返回主目录，否则从当前目录开始\n\
2:ls 路径  用法同上，但路径可置空，代表查找当前目录\n\
3:mkdir 文件名  在当前目录下创建文件目录\n\
4:rmdir用法同上\n\
5:mkfile 文件名 在当前目录下创建文件\n\
6:permission 文件名 0~3 设置文件权限\n\
7:logout 退出登录\n\
注意文件路径使用\\\
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
		cout << "缺少参数" << endl;
		return;
	}
	tmp = readPathFCBFile(command);
	if (tmp == nullptr)
	{
		cout << "路径不正确" << endl;
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
		if (tmp == nullptr) cout << "路径不正确" << endl;
	}
	cout << "文件类型---文件名称------------------------------------------关系--修改时间---------创建时间-------" << endl;
	for (MyFCB* tmpFCB = FCBList->firstFCB; tmpFCB != nullptr; tmpFCB = tmpFCB->next)
	{
		cout << "  ";
		if (tmpFCB->getTypeFlag()) cout << "目录";
		else cout << "文件";
		cout << "      ";
		cout.width(NAME_LEN);
		cout.flags(ios::left);
		cout << tmpFCB->getName();
		cout << "";
		if (!(tmpFCB->getTypeFlag()))
		{
			if ((tmpFCB->getOwner()) == userId) cout << "拥有";
			else switch ((tmpFCB->getIsReadable() ? 1 : 0) * 2 + (tmpFCB->getIsWritable() ? 1 : 0))
			{
			case 0:
				cout << "保护";
				break;
			case 1:
				cout << "只写";
				break;
			case 2:
				cout << "只读";
				break;
			case 3:
				cout << "公有";
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
	if (command.size() > 49) cout << "文件名长度过长" << endl;//-----------------------------使用固定值
	else if (command.empty() || command == "~" || command.find('\\') != -1) cout << "文件名非法" << endl;
	else if (FCBList->findFCB(command) != nullptr) cout << "该文件名已存在" << endl;
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
	if (command.size() > 49) cout << "文件名长度过长" << endl;//-----------------------------使用固定值
	else if (command.empty() || command == "~" || command.find('\\') != -1) cout << "文件名非法" << endl;
	else if (FCBList->findFCB(command) != nullptr) cout << "该文件名已存在" << endl;
	else
	{
		MyFCB* tmp = new MyFCB(false, userId, command);
		tmp->setStorageBlock(findNewBlock());
		//-------------暂未确定文件块索引中父块编号位置
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
	if (command.empty()) cout << "缺少参数" << endl;

	else if (command == path) cout << "不允许删除当前目录" << endl;
	else if (command == "~") cout << "操作非法" << endl;
	else
	{
		tmp = readPathFCB(command);
		if (tmp == nullptr)
		{
			cout << "路径不正确！" << endl;
			return;
		}
		else if (!tmp->firstFCB->getTypeFlag())
		{
			cout << "目标不是一个文件目录" << endl;
			return;
		}
		tmp = readFCBBlocks(tmp->firstFCB->getStorageBlock());
		if (tmp->firstFCB != nullptr)
		{
			cout << "要删除的目录不为空" << endl;
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
		cout << "路径不正确" << endl;
		return;
	}
	tmp = readFCBBlocks(tmp->getParentBlock());
	MyFCB* tmpFCB = tmp->findFCB(command);
	if (tmpFCB->getOwner() != userId)
	{
		cout << "无修改权限" << endl;
	}
	else if (tmpFCB->getTypeFlag())
	{
		cout << "对象为一个目录" << endl;
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
	//char reader[1024];//seek越界不会提示，如果读取出界则在reader中原有的数据不会改变
}
MyFCBHead* File::readPathFCB(string filePath)
{
	string fileName;
	MyFCBHead* tmp = FCBList;
	MyFCB* tmpFCB = nullptr;

	if (filePath == "~" || filePath == "" || filePath[0] == '\\') return nullptr;
	fileName = findFirstFileName(&filePath);
	if (fileName == "~")//表明从根目录起
	{
		tmp = readFCBBlocks(1);
		fileName = findFirstFileName(&filePath);
	}
	while (1) {
		if (fileName=="..")//进入父目录
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
			if (!tmpFCB->getTypeFlag() || tmpFCB->getStorageBlock() == 0) return nullptr;//下一个要打开的目标不为文件夹
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
		output = input->substr(0, pos);//第二个参数为负数则会读到结尾（无效参数？）
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
				//进入下一块/创建新块
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

	//判断原有块是否均已使用，未使用的删除
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
	cout << "请输入用户名：";
	cin >> name;
	user = checkUserName(name);
	if (user == nullptr)
	{
		cout << "用户不存在" << endl;
		return false;
	}
	cout << "请输入密码:";
	cin >> password;
	if (!user->cmpPassword(password))
	{
		cout << "密码错误" << endl;
		return false;
	}
	userId = user->getId();
	userName = user->getName();
	cout << "登录成功" << endl;
	return true;
}
void File::commandShowUser()
{
	User* user;
	cout << "--id----用户名称----------------------------------------------------------" << endl;
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
		cout << "用户数量已达上限" << endl;
		return;
	}
	cout << "请输入用户名：";
	cin >> name;
	if (checkUserName(name) != nullptr)
	{
		cout << "用户名已存在" << endl;
		return;
	}
	cout << "请输入密码:";
	cin >> password;
	writeUser(new User(user->getId(), name, password));
}
void File::commandDeleteUser()
{
	User* user;
	string name, password;
	cout << "请输入用户名：";
	cin >> name;
	user = checkUserName(name);
	if (user == nullptr)
	{
		cout << "用户不存在" << endl;
		return;
	}
	cout << "请输入密码:";
	cin >> password;
	if (!user->cmpPassword(password))
	{
		cout << "密码错误" << endl;
		return;
	}
	deleteUser(user->getId());
	cout << "删除成功" << endl;
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
		while (pos != input.end() && *pos == ' ') pos++;//跳过空格
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
	getline(cin, input);//会正常读取中文字符，处理时可能会出现问题
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
	if (!ioFile.is_open())//无文件，重新创建
	{
		ioFile.open(FILE_NAME, ios::out);//fstream需已只写模式创建文件
		ioFile.close();
		ioFile.open(FILE_NAME);
		//创建管理用户，id为0，名称为root，密码为123456
		for (size_t i = 0; i < BLOCK_SIZE; i++) writeStorage(0, 0, i);
		for (size_t i = 0; i < BLOCK_SIZE; i++) writeStorage(0, 1, i);
		setBlockStage(1, true);
		setSumBlockNum(2);
		User* user=new User(0, "root", "123456");
		writeUser(user);
		cout << "文件创建成功" << endl;
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
	//直观显示模拟内存中数据
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

	//使用16进制ascii码查看器来查看内存内容
	/*ioFile.seekp(blockNum * BLOCK_SIZE + posNum);
	ioFile.put(input);*/
}
inline unsigned char File::readStorage(int blockNum, int posNum)
{
	int output = 0;
	char tmp[2];
	//直观显示模拟内存中数据
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
	//使用16进制ascii码查看器来查看内存内容
	/*ioFile.seekg(blockNum * BLOCK_SIZE + posNum);
	output = (unsigned char)ioFile.get();*/

	return output;
}