#include "User.h"

User::User()
{
}

User::User(int id, string name, string password)
{
	this->id = id;
	for (size_t i = 0; i < 31; i++)this->name[i] = 0;
	for (size_t i = 0; i < name.size(); i++)this->name[i] = name[i];
	for (size_t i = 0; i < 32; i++)this->password[i] = 0;
	for (size_t i = 0; i < password.size(); i++)this->password[i] = password[i];
}

int User::getId()
{
	return id;
}

char* User::toString()
{
	char* output=new char[64];
	output[0] = (char)(id | 16);
	for (size_t i = 0; i < 31; i++) output[i + 1] = name[i];
	for (size_t i = 0; i < 32; i++) output[i + 32] = password[i];
	return output;
}

//id为-1代表用户不存在
User::User(char* input)
{
	if (!(input[0] & 16))
	{
		id = -1;
		return;
	}
	else
	{
		id = (char)(input[0] & 239);
		for (size_t i = 0; i < 31; i++) name[i] = input[i + 1];
		for (size_t i = 0; i < 32; i++) password[i] = input[i + 32];
	}
}

bool User::cmpName(string input)
{
	char name[31] = { 0 };
	for (size_t i = 0; i < input.size(); i++) name[i] = input[i];
	return strcmp(this->name,name)?false:true;
}

bool User::cmpPassword(string input)
{
	char name[30] = { 0 };
	for (size_t i = 0; i < input.size(); i++) password[i] = input[i];
	return strcmp(this->password, password) ? false : true;
}
