#pragma once
#include <iostream>
#include <string>
#include<fstream>
#define CLOCK_SIZE 4096//适配不同页大小暂时不写
using namespace std;

//在文件中以00 00方式代表存储空间中数据

/*一个目录块中0号FCB位置指向上一级文件，上一个块和下一个块（如果有的话）---标记是否已经使用：第一个bit为1代表已使用
* 目录块：64字节为一个FCB块，留空一个FCB块的开头
* 索引块：前二个字节空出
* 文件块：前二个字节空出
*/

//每块4KB---可同时索引2008块，单文件最大约8MB

/*
* FCB结构：64字节
* 创建标记 1bit---已创建为1
* 类型  1bit----文件夹/文件
* 创建者 （编号）4bit------用户添加编号，0~15
* 属性 2bit------类型：只读，只写，保护，公有
* 文件名 47字节------需要控制长度
* 创建时间 6字节
* 修改时间 6字节
* 
*/
//目标块 2字节--------65535块，总大小为256MB
//实际上读取不到就代表没有使用块

class File
{
public:
	void fileControl();//控制界面

private:
	void findFirstCommand();//查找返回字符串中下一个空格前的字符串，返回到command中，如asd 1234 eee 返回asd到command中
	string input;//输入的命令
	string::iterator inputPos;//对输入命令的迭代器指针
	string command;//从命令中分割出的语句
	string path = "现在的路径";//未确定类型

	void loadPath();//加载目录（文件）
	void commandChangePath();//更改目录命令
	fstream ioFile;//文件流

	void fileInitialize();//文件缺失时进行初始化
	void addClock();//初始化一个新的块
	void placeHold(int);//添加指定数目占位符
	int memPos=0;//存储指针
	void writeMem(int);//写入数据到当前位置
	int readMem();//读取当前位置数据返回
};

