#pragma once
#include <iostream>
#include <string>
#include<fstream>
#include <list>
#include "MyFCB.h"
#define BLOCK_SIZE 4096//适配不同页大小暂时不写
using namespace std;

//目前无缓冲区

//在文件中以00 00方式代表存储空间中数据

/*第一个字为1代表已使用
*一个目录块中0号FCB位置指向上一级文件，上一个块和下一个块（如果有的话）---标记是否已经使用：第一个字为1代表已使用，第二个字指向下一块，为0则无下一块
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
* 文件名 49字节------需要控制长度
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
	string path = "~";//未确定类型---~//fileName//file.txt

	list<MyFCB> FCBList;
	bool commandEndFlag;//使用命令过程中终止命令----------------
	void loadPath();//加载目录（文件）
	void commandChangePath();//更改目录命令
	void commandCreatePath();//在当前目录下创建一个文件夹
	fstream ioFile;//文件流

	void fileInitialize();//文件缺失时进行初始化
	void addBlock(int);//初始化一个新的块---------考虑改成按照块号初始化,传入块号
	void placeHold(int);//添加指定数目占位符
	int memPos=0;//存储指针
	void writeMem(int);//写入数据到memPos当前位置
	int readMem();//读取memPos当前位置数据返回

	int locateBlock();//路径存储在command中，定位到路径对应的块，返回块的编号，定位失败返回-1
	void loadFCB(int);//加载指定块和之后相关块中的所有FCB块
};

