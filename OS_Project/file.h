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
* ����  1bit----�ļ���/�ļ�----Ϊ1��Ϊ�ļ���
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
	string::iterator inputPos;//����������ĵ�����ָ��
	string input;//���������
	string command;//�������зָ�������
	string path;//δȷ������---~//fileName//file.txt
	list<MyFCB>* FCBList;
	int currentBlock;//��¼��ǰ��
	int memPos;//�洢ָ��
	fstream ioFile;//�ļ���

	void commandChangePath();//����Ŀ¼����
	void commandShowPathFile();//��ʾcommand�µ�Ŀ¼�е��ļ����ÿմ���Ϊ��ǰ�ļ�·��
	void commandCreatePath();//�ڵ�ǰĿ¼�´���һ���ļ���,��������д��FCBͷ��������ռ�
	void commandDeletePath();//ɾ��ָ����Ŀ¼����Ŀ¼����Ϊ��Ŀ¼

	void loadMainPath();//������Ŀ¼���ļ���
	void findFirstCommand();//���ҷ����ַ�������һ���ո�ǰ���ַ��������ص�command�У�δ�ҵ��᷵�ؿմ�
	pair<int, int> locateBlock();//·���洢��command�У���λ��·����Ӧ�Ŀ飬���ؿ�ı�źͿ�����ͣ�0Ϊ�����飬1ΪĿ¼�飬��λʧ�ܷ���-1������λ���ļ�δ����ռ��Ϊ�����
	string findFileName();//����command�еĵ�һ���ļ��������޸�command�е�����

	void writeMem(unsigned char);//д�����ݵ�memPos��ǰλ�ã�memPos��һ
	int readMem();//��ȡmemPos��ǰλ�����ݷ���,��ȡʧ�ܻ᷵��-1��memPos��һ

	void loadFCB(int);//����ָ�����֮����ؿ��е�����FCB��
	void writeAllFCB();//��FCB���޸�д�ص��洢�У�·��Ϊpath
	void writeFCB(list<MyFCB>::iterator);//д��һ��FCB�鵽memPosλ��

	int newBlock();//����һ��δʹ�õĿ飬�����ʼ����0ռλ����1������ʹ�ã����ؿ��
	void clearBlock(int);//��ʼ��һ���µĿ�---------���տ�ų�ʼ��,������	
	void deleteBlock(int);// ��newBlock��Ӧ
	void setBlockStage(int,bool);//���Ŀ��ʹ��״̬
	void setNextBlock(int,int);//���ĵ�ǰ��ָ�����һ�飨����ΪĿ¼�飩
	int getNextBlock(int);//����Э������˳��Ŀ�
	//��ѡ��������λ�Ѽ���FCB�飬���ظÿ���list��λ��
		//int blockType;//��¼��ǰ���ص�block������1��Ŀ¼�飬0���ļ���
	//��һ����λFCBList���ļ���--locateBlock��commandDeletePath
	void deleteAllNextBlock(int);//ɾ����ǰ��֮�����������Ŀ飬������0������
};

