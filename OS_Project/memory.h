#pragma once
#include <iostream>
#include <string>
using namespace std;

class Memory
{
public:
	void memoryControl();//控制界面

private:
	void findFirstCommand();//查找返回字符串中下一个空格前的字符串，返回到command中，如asd 1234 eee 返回asd到command中
	string input;//输入的命令
	string::iterator inputPos;//对输入命令的迭代器指针
	string command;//从命令中分割出的语句
	string path = "现在的路径";//未确定类型

	void commandChangePath();//更改目录命令
};

