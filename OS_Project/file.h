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

/*��һ����Ϊ1������ʹ��
*һ��Ŀ¼����0��FCBλ��ָ����һ���ļ�����һ�������һ���飨����еĻ���---����Ƿ��Ѿ�ʹ�ã���һ����Ϊ1������ʹ�ã��ڶ�����ָ����һ�飬Ϊ0������һ��
* Ŀ¼�飺64�ֽ�Ϊһ��FCB�飬����һ��FCB��Ŀ�ͷ
* �����飺ǰ�����ֽڿճ�
* �ļ��飺ǰ�����ֽڿճ�
*/

//ÿ��4KB---��ͬʱ����2008�飬���ļ����Լ8MB

/*
* FCB�ṹ��64�ֽ�
* ������� 1bit---�Ѵ���Ϊ1
* ����  1bit----�ļ���/�ļ�
* ������ ����ţ�4bit------�û���ӱ�ţ�0~15
* ���� 2bit------���ͣ�ֻ����ֻд������������
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
	void fileControl();//���ƽ���

private:
	void findFirstCommand();//���ҷ����ַ�������һ���ո�ǰ���ַ��������ص�command�У���asd 1234 eee ����asd��command��
	string input;//���������
	string::iterator inputPos;//����������ĵ�����ָ��
	string command;//�������зָ�������
	string path = "~";//δȷ������---~//fileName//file.txt

	list<MyFCB> FCBList;
	bool commandEndFlag;//ʹ�������������ֹ����----------------
	void loadPath();//����Ŀ¼���ļ���
	void commandChangePath();//����Ŀ¼����
	void commandCreatePath();//�ڵ�ǰĿ¼�´���һ���ļ���
	fstream ioFile;//�ļ���

	void fileInitialize();//�ļ�ȱʧʱ���г�ʼ��
	void addBlock(int);//��ʼ��һ���µĿ�---------���Ǹĳɰ��տ�ų�ʼ��,������
	void placeHold(int);//���ָ����Ŀռλ��
	int memPos=0;//�洢ָ��
	void writeMem(int);//д�����ݵ�memPos��ǰλ��
	int readMem();//��ȡmemPos��ǰλ�����ݷ���

	int locateBlock();//·���洢��command�У���λ��·����Ӧ�Ŀ飬���ؿ�ı�ţ���λʧ�ܷ���-1
	void loadFCB(int);//����ָ�����֮����ؿ��е�����FCB��
};

