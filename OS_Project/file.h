#pragma once
#include <iostream>
#include <string>
#include<fstream>
using namespace std;

/*һ��Ŀ¼����0��FCBλ��ָ����һ���ļ�����һ�������һ���飨����еĻ���---����Ƿ��Ѿ�ʹ�ã���һ��bitΪ1������ʹ��
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
* �ļ��� 47�ֽ�------��Ҫ���Ƴ���
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
	string path = "���ڵ�·��";//δȷ������

	void loadPath();//����Ŀ¼���ļ���
	void commandChangePath();//����Ŀ¼����
	fstream ioFile;//�ļ���
};

