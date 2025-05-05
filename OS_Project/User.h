#pragma once
#include <string>
//#include <cstring>
using namespace std;
/*结构：
* 登录：读取
* 用户条目：（仅0号块）
* 条目有效：4bit 1为有效
* 用户id：4bit 0~16 0号为管理
* 用户名：最大32
* 密码：最大31
* 共64
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

