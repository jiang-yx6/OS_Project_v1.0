#include "MyFCB.h"

MyFCB::MyFCB()
{
	dataFlag=0;
	storageBlock=0;
}

void MyFCB::operator=(const MyFCB& target)
{
	this->dataFlag = target.dataFlag;
	for (size_t i = 0; i < 49; i++) this->name[i] = target.name[i];
	for (size_t i = 0; i < 6; i++) this->createTime[i] = target.createTime[i];
	for (size_t i = 0; i < 6; i++) this->changeTime[i] = target.changeTime[i];
	this->storageBlock = target.storageBlock;
}

void MyFCB::flashTime()
{
	char timeBuffer[80];
	time_t t = time(NULL);
	strftime(timeBuffer, 80, "%Y-%m-%d %H:%M:%S", localtime(&t));
	this->changeTime[0] = ((timeBuffer[0] - 48) * 1000 + (timeBuffer[1] - 48) * 100 + (timeBuffer[2] - 48) * 10 + (timeBuffer[3] - 48)) / 256;
	this->changeTime[1] = ((timeBuffer[0] - 48) * 1000 + (timeBuffer[1] - 48) * 100 + (timeBuffer[2] - 48) * 10 + (timeBuffer[3] - 48)) % 256;
	this->changeTime[2] = (timeBuffer[5] - 48) * 10 + (timeBuffer[6] - 48);
	this->changeTime[3] = (timeBuffer[8] - 48) * 10 + (timeBuffer[9] - 48);
	this->changeTime[4] = (timeBuffer[11] - 48) * 10 + (timeBuffer[12] - 48);
	this->changeTime[5] = (timeBuffer[14] - 48) * 10 + (timeBuffer[15] - 48);
}

string MyFCB::toTime(int type)
{
	unsigned char* aim;
	if (type)
	{
		aim = this->createTime;
	}
	else
	{
		aim = this->changeTime;
	}
	string time;
	time = to_string(aim[0] * 256 + aim[1])+"-"
		+to_string(aim[2]) + "-"
		+ to_string(aim[3]) + " "
		+ to_string(aim[4]) + ":"
		+ to_string(aim[5]);
	return time;
}

void MyFCB::clear()
{
	for (size_t i = 0; i < 49; i++)this->name[i] = '\0';
	this->storageBlock = 0;
}

MyFCB::MyFCB(string name, int flags)
{
	this->dataFlag = (unsigned char)flags;
	for (size_t i = 0; i < 49; i++)this->name[i] = 0;
	for (size_t i = 0; i < name.size(); i++)this->name[i] = name[i];
	flashTime();
	for (size_t i = 0; i < 6; i++) this->createTime[i] = this->changeTime[i];
	storageBlock = 0;
}

