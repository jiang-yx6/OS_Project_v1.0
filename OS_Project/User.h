#pragma once
#include <string>
//#include <cstring>
using namespace std;
/*�ṹ��
* ��¼����ȡ
* �û���Ŀ������0�ſ飩
* ��Ŀ��Ч��4bit 1Ϊ��Ч
* �û�id��4bit 0~16 0��Ϊ����
* �û��������32
* ���룺���31
* ��64
*/
class User
{
	bool enableFlag;
	int id;
	char name[31];
	char password[32];

public:
	User(char*);
	User(int,string="", string="");
	char* toString() const;
	void disableUser();
	bool cmpName(string) const;
	bool cmpPassword(string) const;

	bool getEnableFlag() const;
	int getId() const;
	void setId(int);
	string getName() const;
};

