#include "User.h"

User::User(char* input)
{
	for (size_t i = 0; i < 31; i++)name[i] = 0;
	for (size_t i = 0; i < 32; i++)password[i] = 0;
	if (enableFlag = (input[0] & 0b00010000 ? true : false))
	{
		id = (input[0] & 0b00001111);
		for (size_t i = 0; i < 31; i++) name[i] = input[i + 1];
		for (size_t i = 0; i < 32; i++) password[i] = input[i + 32];
	}
}
User::User(int id, string name, string password)
{
	enableFlag = true;
	this->id = id;
	for (size_t i = 0; i < 31; i++)this->name[i] = 0;
	for (size_t i = 0; i < name.size(); i++)this->name[i] = name[i];
	for (size_t i = 0; i < 32; i++)this->password[i] = 0;
	for (size_t i = 0; i < password.size(); i++)this->password[i] = password[i];
}
char* User::toString() const
{
	char* output = new char[64];
	output[0] = (char)(id + (enableFlag ? 16 : 0));
	for (size_t i = 0; i < 31; i++) output[i + 1] = name[i];
	for (size_t i = 0; i < 32; i++) output[i + 32] = password[i];
	return output;
}
void User::disableUser()
{
	enableFlag = false;
}
bool User::cmpName(string input) const
{
	if (input.size() > 31) return false;
	char name[31] = { 0 };
	for (size_t i = 0; i < input.size(); i++) name[i] = input[i];
	return strcmp(this->name, name) ? false : true;
}
bool User::cmpPassword(string input) const
{
	if (input.size() > 32) return false;
	char password[32] = { 0 };
	for (size_t i = 0; i < input.size(); i++) password[i] = input[i];
	return strcmp(this->password, password) ? false : true;
}

bool User::getEnableFlag() const
{
	return enableFlag;
}
int User::getId() const
{
	return id;
}
void User::setId(int id)
{
	this->id = id;
}
string User::getName() const
{
	string output = "";
	for (size_t i = 0; i < strlen(name); i++) output += name[i];
	return output;
}