#pragma once
#include <iostream>
#include <string>
#include<fstream>
#include <list>
#include <conio.h>
#include <sstream>

#include "MyFCB.h"
#include "User.h"
#include "process/process.h"
#define BLOCK_SIZE 4096//适配不同页大小暂时不写
#define BLOCK_ADD_LEN 2//用于定位块的字节数
#define FILE_NAME "storage.txt"//文件名
using namespace std;

//目前无缓冲区
//一个程序打开文件，另一个程序删除文件---出错，暂不考虑

//考虑写一个char数组和string转换的函数
//考虑给文件夹名字加发光

/*第一个字为1代表已使用
* 目录块：目录块：64字节为一个FCB块，留空一个FCB块的开头，前两个字节空出
* 索引块：前四个字节空出
* 文件块：前二个字节空出
*/

/*单文件索引块：
* 不分级
* 第一个字为块使用符号
* 第二个字为父块块号
* 三，四个字为文件总长度
*/

//每块4KB---可同时索引2008块，单文件最大约8MB
//目标块 2字节--------65535块，总大小为256MB

class File
{
public:
	File();
private:
	ProcessManager pm;

	//以选定条件定位已加载FCB块，返回该块在list的位置
	//int blockType;//记录当前加载的block块的类别，1：目录块，0：文件块
	//加一个定位FCBList中文件名--locateBlock和commandDeletePath
	//检测字符串分段数目
	//修改所有使用cin的语句
	//需防止删掉打开的文件
public:
	//登录控制
	void loginIn();

	//文件控制界面
	void fileControl();
	/*										*/	
	//更改目录命令
	void commandChangePath(string);
	//显示input下的目录中的文件，置空代表为当前文件路径
	void commandShowPathFile(string);
	//在当前目录下创建一个文件夹,仅创建并写入FCB头，不分配空间
	void commandCreatePath(string);
	//创建文件
	void commandCreateFile(string);
	//删除指定的目录，该目录必须为空目录
	void commandDeletePath(string);
	//删除指定文件
	void commandDeleteFile(string);
	//更改文件权限
	void commandChangePermission(string);
	//向文件写入内容
	void commandWriteFile(string);
	//输出文件内容
	void commandShowFile(string);
	//模拟VIM功能
	void commandVim(string);

	std::string getSecondPart(const std::string& input);
private:
	//记录主路径
	string path;//-------------------------------可能会出问题
	//记录当前路径下FCB内容
	MyFCBHead* FCBList;
	//记录当前用户名
	string userName;
	//记录当前用户id
	int userId;

	//加载主目录（文件）
	void loadMainPath();
	//读取路径指向的FCB块，返回到firstFCB中，返回其所在块到currentBlock中，返回nullptr为读取失败，根目录无FCB，不应读取
	MyFCBHead* readPathFCB(string);
	//读取路径指向的文件夹，返回FCBList
	MyFCBHead* readPathFCBFile(string);
	//查找input中的第一个文件名
	string findFirstFileName(string*);
	//向blockNum块连续写入input的所有内容，使用后应当使用readFCBBlocks来刷新input中数据
	void writeFCBBlocks(MyFCBHead*);
	//读取blockNum中的目录返回
	MyFCBHead* readFCBBlocks(int);

	//登录，成功则返回true并设置userName和userId，失败返回false
	bool commandLogin();
	//显示所有已创建的用户的名称与id
	void commandShowUser();
	//尝试创建一个新用户
	void commandCreateUser();
	//尝试删除一个已有用户
	void commandDeleteUser();
	//检测名称为input的用户，不存在则返回nullptr
	User* checkUserName(string);
	//查找出为被使用的id创建User，查找失败返回nullptr
	User* getNewUser();
	/*										*/
	//查找input中第num个字符串，写入到command中
	string findString(int,string);

	//输入文件索引所在的块号以及文件指定起始位置字符的编号与读取长度，返回读取的结果，出错返回nullptr，不足则以'\0'补充
	char* readFileLine(int, int, int);
	//显示文件内容到屏幕上
	void showFile(int blockNum);
	//向文件中指定位置添加内容
	void addFileData(int,int,char);
	//删除文件中指定位置的内容
	void delFileData(int,int);
	//设置文件总长度加1，按需新建新块
	void addFileTotalLen(int);
	//设置文件总长度减1，同时删除多余的块
	void subFileTotalLen(int);
	//读取返回文件中指定位置的内容
	unsigned char readFile(int, int);
	//向文件中指定位置写入内容
	void writeFile(int,int,unsigned char);
	//获取文件总长度
	int getFileTotalLen(int);
	//设置文件总长度
	void setFileTotalLen(int,int);
	//向文件最后添加块
	void addBlock(int);
	//删除文件最后的块
	void delBlock(int);
	//设置文件路径记录blockNum中文件的父块
	void setFileParentBlock(int,int);
	//获取文件路径记录blockNum中文件的父块
	int getFileParentBlock(int);
	//定位pos在文件中对应的块
	int locateFileBlock(int,int);
	//定位pos在文件中对应的块中的位置，编号从0开始
	int locateFilePos(int,int);

	//向blockNum块FCBNum位置写入FCB，编号为0~64
	void writeFCB(int, int, MyFCB*);
	//读出blockNum块FCBNum位置的FCB返回
	MyFCB* readFCB(int, int);
	//删除当前块及之后的FCB块，若删除第一块需注意清除FCB中storageBlock
	void deleteFCBBlock(int);
	//设置block块的下一个块
	void setNextFCBBlock(int, int);
	//读取block块的下一个块，为0则不存在
	int getNextFCBBlock(int);
	//设置路径记录blockNum中目录的父块
	void setPathParentBlock(int, int);
	//获取路径记录blockNum中目录的父块
	int getPathParentBlock(int);

	//删除指定id处user的数据
	void deleteUser(int);
	//读取指定id对应位置的user
	User* readUser(int);
	//将输入的user写入到id对应的位置
	void writeUser(User*);
	/*										*/
	//读取输入返回到output中，返回值为字符串总长度
	string getCommand();

	//查找到第一个未使用的块，初始化并返回块号
	int findNewBlock();
	//写入到指定块
	void writeBlock(char*, int);
	//读取指定块，注意释放内存
	char* readBlock(int);
	//清空编号为blockNum的块
	void clearBlock(int);
	//向blockNum块第posNum位置写入长度为len的input,posNum+len不可超过BLOCK_SIZE
	void writeLine(char*, int, int, int);
	//读取blockNum块第posNum位置长度为len的数据返回,posNum+len不可超过BLOCK_SIZE
	char* readLine(int, int, int);
	//获取已创建总块数，仅存储于1号块parentBlock位置
	int getSumBlockNum();
	//设置已创建总块数，仅存储于1号块parentBlock位置
	void setSumBlockNum(int);
	//设置编号为blockNum的块的使用状态为status
	void setBlockStage(int, bool);
	//获取编号为blockNum的块的使用状态
	bool getBlockStage(int);
	/*										*/
	//文件流
	fstream ioFile;

	//char数组转换为string
	string charToString(char*);
	//创建文件，初始化格式
	void tryOpenFile();
	//在模拟磁盘文件中创建新的部分
	int createBlock();
	//写入input到blockNum块第posNum位置
	void writeStorage(unsigned char, int, int);
	//读取blockNum块第posNum位置数据返回
	unsigned char readStorage(int, int);
};