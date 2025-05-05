#pragma once
#include <ctime>
#include <string>
#define NAME_LEN 49
using namespace std;
/*
* FCB结构：64字节
* 创建标记 1bit---已创建为1
* 类型  1bit----文件夹/文件
* 属性 2bit------类型：只读，只写，保护，公有：10，01，00，11---默认为保护(对于文件有效，文件夹各用户均可开启)
* 创建者 （编号）4bit------用户添加编号，0~15,0为管理权限
* 文件名 49字节------需要控制长度
* 创建时间 6字节
* 修改时间 6字节
* 指向目录 2字节
*/
class MyFCB
{
	static char* getCharTime();
	static string charToTime(unsigned char*);
	bool createFlag;
	//为false则为文件，为true则为文件夹
	bool typeFlag;
	bool isReadable;
	bool isWritable;
	int owner;
	char name[NAME_LEN];
	unsigned char createTime[6];
	unsigned char changeTime[6];
	int storageBlock;
public:
	MyFCB* next = nullptr;

	MyFCB(char*);
	MyFCB(bool, int, string);
	char* toString() const;
	void deleteFCB();
	bool getCreateFlag() const;
	bool getTypeFlag() const;
	void setIsReadable(bool);
	bool getIsReadable() const;
	void setIsWritable(bool);
	bool getIsWritable() const;
	int getOwner() const;
	string getName() const;
	string getCreateTime();
	void flashTime();
	string getChangeTime();
	void setStorageBlock(int);
	int getStorageBlock() const;
};


/*位于一个目录块中0号FCB位置
* 使用标记：两字节
* 下一块标记，若为0则为最后一块：两字节
* parentBlock指向上一级文件，若为0则无上级文件：两字节
* firstFCB指向第一个FCB
*/
class MyFCBHead
{
public:
	MyFCBHead(int);
	MyFCB* firstFCB = nullptr;
	MyFCB* findFCB(string) const;
	MyFCB* findFCB(int) const;
	void delFCB(int);
	void addFCB(MyFCB*);
	void setParentBlock(int);
	int getParentBlock() const;
	int getCurrentBlock() const;

private:
	int parentBlock;
	int currentBlock;
};