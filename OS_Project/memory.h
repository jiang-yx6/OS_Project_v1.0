#pragma once
#include <iostream>
#include <string>
using namespace std;

class Memory
{
public:
	void memoryControl();//���ƽ���

private:
	void findFirstCommand();//���ҷ����ַ�������һ���ո�ǰ���ַ��������ص�command�У���asd 1234 eee ����asd��command��
	string input;//���������
	string::iterator inputPos;//����������ĵ�����ָ��
	string command;//�������зָ�������
	string path = "���ڵ�·��";//δȷ������

	void commandChangePath();//����Ŀ¼����
};

