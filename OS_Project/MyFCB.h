#pragma once
#include <ctime>
#include <string>
#define NAME_LEN 49
using namespace std;
/*
* FCB�ṹ��64�ֽ�
* ������� 1bit---�Ѵ���Ϊ1
* ����  1bit----�ļ���/�ļ�
* ���� 2bit------���ͣ�ֻ����ֻд�����������У�10��01��00��11---Ĭ��Ϊ����(�����ļ���Ч���ļ��и��û����ɿ���)
* ������ ����ţ�4bit------�û���ӱ�ţ�0~15,0Ϊ����Ȩ��
* �ļ��� 49�ֽ�------��Ҫ���Ƴ���
* ����ʱ�� 6�ֽ�
* �޸�ʱ�� 6�ֽ�
* ָ��Ŀ¼ 2�ֽ�
*/
class MyFCB
{
	static char* getCharTime();
	static string charToTime(unsigned char*);
	bool createFlag;
	//Ϊfalse��Ϊ�ļ���Ϊtrue��Ϊ�ļ���
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


/*λ��һ��Ŀ¼����0��FCBλ��
* ʹ�ñ�ǣ����ֽ�
* ��һ���ǣ���Ϊ0��Ϊ���һ�飺���ֽ�
* parentBlockָ����һ���ļ�����Ϊ0�����ϼ��ļ������ֽ�
* firstFCBָ���һ��FCB
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