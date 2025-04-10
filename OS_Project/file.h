#pragma once
#include <iostream>
#include <string>
#include<fstream>
#include <list>
#include "MyFCB.h"
#define BLOCK_SIZE 4096//适配不同页大小暂时不写
#define FCB_SIZE 64//FCB块大小
#define BLOCK_ADD_LEN 2//用于定位块的字节数
#define FILE_NAME "storage.txt"//文件名
using namespace std;

//目前无缓冲区

//在文件中以00 00方式代表存储空间中数据

//考虑写一个char数组和string转换的函数
//考虑给文件夹名字加发光

/*第一个字为1代表已使用
*一个目录块中0号FCB位置指向上一级文件，下一个块（如果有的话）---标记是否已经使用：第一个字为1代表已使用，第二个字指向下一块，为0则无下一块
* 目录块：64字节为一个FCB块，留空一个FCB块的开头
* 索引块：前二个字节空出
* 文件块：前二个字节空出
*/

//每块4KB---可同时索引2008块，单文件最大约8MB

/*
* FCB结构：64字节
* 创建标记 1bit---已创建为1
* 类型  1bit----文件夹/文件----为1则为文件夹
* 属性 2bit------类型：只读，只写，保护，公有：10，01，00，11---默认为保护
* 创建者 （编号）4bit------用户添加编号，0~15,0为管理权限
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
	File();
	//控制界面
	void fileControl();
	//读取指定块并返回,不应该会读到错误的块，调用者记得释放内存
	char* readBlock(int);
	//写入到指定块
	void writeBlock(char*, int);
	//查找一个未使用的块，将其初始化：0占位，设1代表已使用，返回块号
	int newBlock();
	//初始化一个新的块---------按照块号初始化,传入块号
	void clearBlock(int);	
	// 与newBlock对应
	void deleteBlock(int);
	//删除当前块之后所有连续的块，若传入0则跳过
	void deleteAllNextBlock(int);
	//更改块的使用状态
	void setBlockStage(int, bool);
	//获取块的使用状态
	bool getBlockStage(int);
	//更改当前块指向的下一块（必须为目录块）
	void setNextBlock(int, int);
	//用于协助遍历顺序的块
	int getNextBlock(int);

	//更改目录命令
	void commandChangePath();

private:
	//写入数据到storagePos当前位置，storagePos加一
	void writeStorage(unsigned char);
	//读取storagePos当前位置数据返回,读取失败会返回-1，storagePos加一
	int readStorage();
	//对输入命令的迭代器指针
	string::iterator inputPos;
	//输入的命令
	string input;
	//从命令中分割出的语句
	string command;
	//未确定类型---~//fileName//file.txt
	string path;
	list<MyFCB>* FCBList;
	//记录当前块
	int currentBlock;
	//存储指针
	int storagePos;
	//文件流
	fstream ioFile;
	//记录当前用户的主路径
	string userPath;
	//加载主目录（文件）------------------------分用户待适配
	void loadMainPath();
	//查找返回字符串中下一个空格前的字符串，返回到command中，未找到会返回空串
	void findFirstCommand();
	//查找command中的第一个文件名------------------------！！！会修改command中的内容！！！
	string findFileName();

	//显示command下的目录中的文件，置空代表为当前文件路径
	void commandShowPathFile();
	//在当前目录下创建一个文件夹,仅创建并写入FCB头，不分配空间
	void commandCreatePath();
	//删除指定的目录，该目录必须为空目录
	void commandDeletePath();
	//路径存储在command中，定位到路径对应的块，返回块的编号和块的类型：0为索引块，1为目录块，定位失败返回-1，若定位的文件未分配空间会为其分配
	pair<int, int> locateBlock();
	//加载指定块和之后相关块中的所有FCB块-----------------调用了redMem，待修改
	void loadFCB(int);
	//将FCB的修改写回到存储中，路径为path
	void writeAllFCB();
	//写入一个FCB块到storagePos位置--------------调用了writeMem，待修改
	void writeFCB(list<MyFCB>::iterator);

	//添加一个readFCB与writeFCB对应
	//以选定条件定位已加载FCB块，返回该块在list的位置
		//int blockType;//记录当前加载的block块的类别，1：目录块，0：文件块
	//加一个定位FCBList中文件名--locateBlock和commandDeletePath
};