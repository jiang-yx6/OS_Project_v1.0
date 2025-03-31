#pragma once
#include <iostream>
#include <string>
#include<fstream>
#include <list>
#include "MyFCB.h"
#define BLOCK_SIZE 4096//���䲻ͬҳ��С��ʱ��д
using namespace std;

//Ŀǰ�޻�����

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
* ����  1bit----�ļ���/�ļ�
* ���� 2bit------���ͣ�ֻ����ֻд�����������У�10��01��00��11---Ĭ��Ϊ����
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
	void fileControl();//���ƽ���

private:
	void findFirstCommand();//���ҷ����ַ�������һ���ո�ǰ���ַ��������ص�command�У���asd 1234 eee ����asd��command��
	string input;//���������
	string::iterator inputPos;//����������ĵ�����ָ��
	string command;//�������зָ�������
	string path;//δȷ������---~//fileName//file.txt

	list<MyFCB>* FCBList;
	//bool commandEndFlag;//ʹ�������������ֹ����----------------
	void loadMainPath();//����Ŀ¼���ļ���
	void commandChangePath();//����Ŀ¼����
	void commandShowPathFile();//��ʾcommand�µ�Ŀ¼�е��ļ�----��ȷ��ָ����Ϊ�ļ���
	void commandCreatePath();//�ڵ�ǰĿ¼�´���һ���ļ���,��������д��FCBͷ��������ռ�
	fstream ioFile;//�ļ���

	//void fileInitialize();//�ļ�ȱʧʱ���г�ʼ��
	void clearBlock(int);//��ʼ��һ���µĿ�---------���տ�ų�ʼ��,������
	void placeHold(int);//���ָ����Ŀռλ��
	int memPos;//�洢ָ��
	void writeMem(char);//д�����ݵ�memPos��ǰλ��
	int readMem();//��ȡmemPos��ǰλ�����ݷ���,��ȡʧ�ܻ᷵��-1

	int currentBlock;//��¼��ǰ��
	int locateBlock();//·���洢��command�У���λ��·����Ӧ�Ŀ飬���ؿ�ı�ţ���λʧ�ܷ���-1
	void loadFCB(int);//����ָ�����֮����ؿ��е�����FCB��
	void writeAllFCB();//��FCB���޸�д�ص��洢��
	int newBlock();//����һ��δʹ�õĿ飬�����ʼ����0ռλ����1������ʹ��
	void writeFCB(list<MyFCB>::iterator);//д��һ��FCB�鵽memPosλ��
};

