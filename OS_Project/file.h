#pragma once
#include <iostream>
#include <string>
#include<fstream>
#include <list>
#include "MyFCB.h"
#include "User.h"
#include "process/process.h"
#define BLOCK_SIZE 4096//���䲻ͬҳ��С��ʱ��д
#define FCB_SIZE 64//FCB���С
#define BLOCK_ADD_LEN 2//���ڶ�λ����ֽ���
#define FILE_NAME "storage.txt"//�ļ���
using namespace std;

//Ŀǰ�޻�����

/*��¼����ȡ
* �û���Ŀ������0�ſ飩
* ��Ŀ��Ч��4bit 1Ϊ��Ч
* �û�id��4bit 0~16 0��Ϊ����
* �û��������32
* ���룺���31
* ��64
*/


//���ļ�����00 00��ʽ����洢�ռ�������

//����дһ��char�����stringת���ĺ���
//���Ǹ��ļ������ּӷ���

/*��һ����Ϊ1������ʹ��
*һ��Ŀ¼����0��FCBλ��ָ����һ���ļ�����һ���飨����еĻ���---����Ƿ��Ѿ�ʹ�ã���һ����Ϊ1������ʹ�ã��ڶ�����ָ����һ�飬Ϊ0������һ��
* Ŀ¼�飺64�ֽ�Ϊһ��FCB�飬����һ��FCB��Ŀ�ͷ
* �����飺ǰ�����ֽڿճ�
* �ļ��飺ǰ�����ֽڿճ�
*/

//ÿ��4KB---��ͬʱ����2008�飬���ļ����Լ8MB

/*
* FCB�ṹ��64�ֽ�
* ������� 1bit---�Ѵ���Ϊ1
* ����  1bit----�ļ���/�ļ�----Ϊ1��Ϊ�ļ���
* ���� 2bit------���ͣ�ֻ����ֻд�����������У�10��01��00��11---Ĭ��Ϊ����(�����ļ���Ч���ļ��и��û����ɿ���)
* ������ ����ţ�4bit------�û���ӱ�ţ�0~15,0Ϊ����Ȩ��
* �ļ��� 49�ֽ�------��Ҫ���Ƴ���
* ����ʱ�� 6�ֽ�
* �޸�ʱ�� 6�ֽ�
* 
*/
//Ŀ��� 2�ֽ�--------65535�飬�ܴ�СΪ256MB
//ʵ���϶�ȡ�����ʹ���û��ʹ�ÿ�

class File
{
public:
	File();
	//���ƽ���
	void fileControl();
	//��ȡָ���鲢����,��Ӧ�û��������Ŀ飬�����߼ǵ��ͷ��ڴ�
	char* readBlock(int);
	//д�뵽ָ����
	void writeBlock(char*, int);
	//����һ��δʹ�õĿ飬�����ʼ����0ռλ����1������ʹ�ã����ؿ��
	int newBlock();
	//��ʼ��һ���µĿ�---------���տ�ų�ʼ��,������
	void clearBlock(int);	
	// ��newBlock��Ӧ
	void deleteBlock(int);
	//ɾ����ǰ��֮�����������Ŀ飬������0������
	void deleteAllNextBlock(int);
	//���Ŀ��ʹ��״̬
	void setBlockStage(int, bool);
	//��ȡ���ʹ��״̬
	bool getBlockStage(int);
	//���ĵ�ǰ��ָ�����һ�飨����ΪĿ¼�飩
	void setNextBlock(int, int);
	//����Э������˳��Ŀ�
	int getNextBlock(int);

	//����Ŀ¼����
	void commandChangePath();


	string userName;
	int userId;
	int getNewId();
	void deleteUser(int);
	int checkUserName(string);
	char* readUser(int);
	void writeUser(User);
	void newNameBlock();
	void loginIn();
	//д�����ݵ�storagePos��ǰλ�ã�storagePos��һ
	void writeStorage(unsigned char);
	//��ȡstoragePos��ǰλ�����ݷ���,��ȡʧ�ܻ᷵��-1��storagePos��һ
	int readStorage();
	//����������ĵ�����ָ��
	string::iterator inputPos;
	//���������
	string input;
	//�������зָ�������
	string command;
	//δȷ������---~//fileName//file.txt
	string path;
	list<MyFCB>* FCBList;
	//��¼��ǰ��
	int currentBlock;
	//�洢ָ��
	int storagePos;
	//�ļ���
	fstream ioFile;
	//��¼��ǰ�û�����·��
	string userPath;
	//������Ŀ¼���ļ���------------------------���û�������
	void loadMainPath();
	//���ҷ����ַ�������һ���ո�ǰ���ַ��������ص�command�У�δ�ҵ��᷵�ؿմ�
	void findFirstCommand();
	//����command�еĵ�һ���ļ���------------------------���������޸�command�е����ݣ�����
	string findFileName();

	//��ʾcommand�µ�Ŀ¼�е��ļ����ÿմ���Ϊ��ǰ�ļ�·��
	void commandShowPathFile();
	//�ڵ�ǰĿ¼�´���һ���ļ���,��������д��FCBͷ��������ռ�
	void commandCreatePath();
	void commandCreateFile();
	//ɾ��ָ����Ŀ¼����Ŀ¼����Ϊ��Ŀ¼
	void commandDeletePath();
	void commandChangePermission();
	//·���洢��command�У���λ��·����Ӧ�Ŀ飬���ؿ�ı�źͿ�����ͣ�0Ϊ�����飬1ΪĿ¼�飬��λʧ�ܷ���-1������λ���ļ�δ����ռ��Ϊ�����
	pair<int, int> locateBlock();
	//����ָ�����֮����ؿ��е�����FCB��-----------------������redMem�����޸�
	void loadFCB(int);
	//��FCB���޸�д�ص��洢�У�·��Ϊpath
	void writeAllFCB();
	//д��һ��FCB�鵽storagePosλ��--------------������writeMem�����޸�
	void writeFCB(list<MyFCB>::iterator);

	MyFCB* locateNowFCB();

	//���һ��readFCB��writeFCB��Ӧ
	//��ѡ��������λ�Ѽ���FCB�飬���ظÿ���list��λ��
		//int blockType;//��¼��ǰ���ص�block������1��Ŀ¼�飬0���ļ���
	//��һ����λFCBList���ļ���--locateBlock��commandDeletePath
};