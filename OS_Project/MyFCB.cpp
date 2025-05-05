#include "MyFCB.h"

inline char* MyFCB::getCharTime()
{
	char timeBuffer[80];
	char* output = new char[6];
	time_t t = time(NULL);
	strftime(timeBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&t));
	output[0] = ((timeBuffer[0] - 48) * 1000 + (timeBuffer[1] - 48) * 100 + (timeBuffer[2] - 48) * 10 + (timeBuffer[3] - 48)) / 256;
	output[1] = ((timeBuffer[0] - 48) * 1000 + (timeBuffer[1] - 48) * 100 + (timeBuffer[2] - 48) * 10 + (timeBuffer[3] - 48)) % 256;
	output[2] = (timeBuffer[5] - 48) * 10 + (timeBuffer[6] - 48);
	output[3] = (timeBuffer[8] - 48) * 10 + (timeBuffer[9] - 48);
	output[4] = (timeBuffer[11] - 48) * 10 + (timeBuffer[12] - 48);
	output[5] = (timeBuffer[14] - 48) * 10 + (timeBuffer[15] - 48);
	return output;
}
string MyFCB::charToTime(unsigned char* time)
{
	string output;
	output = to_string(time[0] * 256 + time[1]) + "-"
		+ to_string(time[2]) + "-"
		+ to_string(time[3]) + " "
		+ to_string(time[4]) + ":"
		+ to_string(time[5]);
	return output;
}

MyFCB::MyFCB(char* input)
{
	createFlag = input[0] & 0b10000000;
	typeFlag = input[0] & 0b01000000;
	isReadable = input[0] & 0b00100000;
	isWritable = input[0] & 0b00010000;
	owner = input[0] & 0b00001111;
	for (size_t i = 0; i < NAME_LEN; i++) name[i] = input[i + 1];
	for (size_t i = 0; i < 6; i++) createTime[i] = input[i + 50];
	for (size_t i = 0; i < 6; i++) changeTime[i] = input[i + 56];
	storageBlock = ((unsigned char)input[62] << 8) + (unsigned char)input[63];
}
MyFCB::MyFCB(bool type, int owner, string inputName)
{
	createFlag = true;
	typeFlag = type;
	isReadable = false;
	isWritable = false;
	this->owner = owner;
	for (size_t i = 0; i < 49; i++)this->name[i] = 0;
	for (size_t i = 0; i < inputName.size(); i++)this->name[i] = inputName[i];
	flashTime();
	for (size_t i = 0; i < 6; i++) this->createTime[i] = this->changeTime[i];
	storageBlock = 0;
}
char* MyFCB::toString() const
{
	char* output = new char[64];
	output[0] = ((createFlag ? 1 : 0) << 7) + ((typeFlag ? 1 : 0) << 6) + ((isReadable ? 1 : 0) << 5) + ((isWritable ? 1 : 0) << 4) + owner;
	for (size_t i = 0; i < 49; i++) output[i + 1] = name[i];
	for (size_t i = 0; i < 6; i++) output[i + 50] = createTime[i];
	for (size_t i = 0; i < 6; i++) output[i + 56] = changeTime[i];
	output[62] = (unsigned char)storageBlock / 256;
	output[63] = (unsigned char)storageBlock % 256;
	return output;
}
void MyFCB::deleteFCB()
{
	createFlag = false;
}
bool MyFCB::getCreateFlag() const
{
	return createFlag;
}
bool MyFCB::getTypeFlag() const
{
	return typeFlag;
}
void MyFCB::setIsReadable(bool isReadable)
{
	this->isReadable = isReadable;
}
bool MyFCB::getIsReadable() const
{
	return isReadable;
}
void MyFCB::setIsWritable(bool isWritable)
{
	this->isWritable = isWritable;
}
bool MyFCB::getIsWritable() const
{
	return isWritable;
}
int MyFCB::getOwner() const
{
	return owner;
}
string MyFCB::getName() const
{
	string output = "";
	for (size_t i = 0; i < strlen(name); i++) output += name[i];
	return output;
}
string MyFCB::getCreateTime()
{
	return charToTime(createTime);
}
void MyFCB::flashTime()
{
	char* time = getCharTime();
	for (size_t i = 0; i < 6; i++) changeTime[i] = time[i];
}
string MyFCB::getChangeTime()
{
	return charToTime(changeTime);
}
void MyFCB::setStorageBlock(int storageBlock)
{
	this->storageBlock = storageBlock;
}
int MyFCB::getStorageBlock() const
{
	return storageBlock;
}

MyFCBHead::MyFCBHead(int blockNum)
{
	currentBlock = blockNum;
}
MyFCB* MyFCBHead::findFCB(string name) const
{
	MyFCB* tmp = firstFCB;
	while (tmp != nullptr)
	{
		if (tmp->getName() == name) return tmp;
		tmp = tmp->next;
	}
	return nullptr;
}
MyFCB* MyFCBHead::findFCB(int blockNum) const
{
	MyFCB* tmp = firstFCB;
	while (tmp != nullptr)
	{
		if (tmp->getStorageBlock() == blockNum) return tmp;
		tmp = tmp->next;
	}
	return nullptr;
}
void MyFCBHead::delFCB(int aimBlock)
{
	MyFCB* tmp = nullptr;
	if (firstFCB == nullptr) return;
	tmp = firstFCB;
	if (tmp->getStorageBlock()==aimBlock)
	{
		firstFCB = tmp->next;
		return;
	}
	while (tmp->next!=nullptr)
	{
		if (tmp->next->getStorageBlock()==aimBlock)
		{
			tmp->next = tmp->next->next;
			return;
		}
		tmp = tmp->next;
	}
}
void MyFCBHead::addFCB(MyFCB* input)
{
	input->next = firstFCB;
	firstFCB = input;
}
void MyFCBHead::setParentBlock(int parentBlock)
{
	this->parentBlock = parentBlock;
}
int MyFCBHead::getParentBlock() const
{
	return parentBlock;
}
int MyFCBHead::getCurrentBlock() const
{
	return currentBlock;
}
