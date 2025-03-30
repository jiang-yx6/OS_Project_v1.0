#pragma once
#include <ctime>
#include <string>
using namespace std;

class MyFCB
{
public:
	char dataFlag;
	char name[49];
	char createTime[6];
	char changeTime[6];
	int storageBlock;

	MyFCB();
	MyFCB(string, char);
	void operator=(const MyFCB&);
	void flashTime();
	void clear();
};

