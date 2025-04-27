#pragma once
#include <string>
using namespace std;

class User
{
	unsigned char id;
	char password[32];

public:
	char name[31];
	User();
	User(int,string,string);
	int getId();
	char* toString();
	User(char*);
	bool cmpName(string);
	bool cmpPassword(string);
};

