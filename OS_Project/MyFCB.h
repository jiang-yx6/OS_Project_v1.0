#pragma once
#include <ctime>
#include <string>
using namespace std;

class MyFCB
{
public:
	unsigned char dataFlag;
	char name[49];
	unsigned char createTime[6];
	unsigned char changeTime[6];
	unsigned short int storageBlock;

	MyFCB();
	MyFCB(string, int);//int��Ҫ׼������û�id
	void operator=(const MyFCB&);
	void flashTime();
	string toTime(int);//Ϊ0���changetime���������createtime
	void clear();
};

