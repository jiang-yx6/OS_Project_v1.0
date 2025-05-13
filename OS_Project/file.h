#pragma once
#include <iostream>
#include <string>
#include<fstream>
#include <list>
#include "MyFCB.h"
#include "User.h"
#include "process/process.h"
#define BLOCK_SIZE 4096//���䲻ͬҳ��С��ʱ��д
#define BLOCK_ADD_LEN 2//���ڶ�λ����ֽ���
#define FILE_NAME "storage.txt"//�ļ���
using namespace std;

//Ŀǰ�޻�����
//һ��������ļ�����һ������ɾ���ļ�---�����ݲ�����

//����дһ��char�����stringת���ĺ���
//���Ǹ��ļ������ּӷ���

/*��һ����Ϊ1������ʹ��
* Ŀ¼�飺Ŀ¼�飺64�ֽ�Ϊһ��FCB�飬����һ��FCB��Ŀ�ͷ��ǰ�����ֽڿճ�
* �����飺ǰ�����ֽڿճ�
* �ļ��飺ǰ�����ֽڿճ�
*/

/*���ļ������飺
* ���ּ�
* ��һ����Ϊ��ʹ�÷���
* �ڶ�����Ϊ������
*/

//ÿ��4KB---��ͬʱ����2008�飬���ļ����Լ8MB
//Ŀ��� 2�ֽ�--------65535�飬�ܴ�СΪ256MB

class File
{
public:
	File();

	//��ѡ��������λ�Ѽ���FCB�飬���ظÿ���list��λ��
	//int blockType;//��¼��ǰ���ص�block������1��Ŀ¼�飬0���ļ���
	//��һ����λFCBList���ļ���--locateBlock��commandDeletePath
	//����ַ����ֶ���Ŀ
	//�޸�����ʹ��cin�����
	//���ֹɾ���򿪵��ļ�

	//��¼����
	void loginIn();

	//�ļ����ƽ���
	void fileControl();
	/*										*/	
	//����Ŀ¼����
	void commandChangePath();
	//��ʾinput�µ�Ŀ¼�е��ļ����ÿմ���Ϊ��ǰ�ļ�·��
	void commandShowPathFile();
	//�ڵ�ǰĿ¼�´���һ���ļ���,��������д��FCBͷ��������ռ�
	void commandCreatePath();
	//�����ļ�
	void commandCreateFile();
	//ɾ��ָ����Ŀ¼����Ŀ¼����Ϊ��Ŀ¼
	void commandDeletePath();
	//�����ļ�Ȩ��
	void commandChangePermission();
	//��¼��·��
	string path;
	//��¼��ǰ·����FCB����
	MyFCBHead* FCBList;
	//��¼��ǰ�û���
	string userName;
	//��¼��ǰ�û�id
	int userId;
	//��input��ȡ��������
	string command;

	//������Ŀ¼���ļ���
	void loadMainPath();
	//��ȡ·��ָ���FCB�飬���ص�firstFCB�У����������ڿ鵽currentBlock�У�����nullptrΪ��ȡʧ�ܣ���Ŀ¼��FCB����Ӧ��ȡ
	MyFCBHead* readPathFCB(string);
	//��ȡ·��ָ����ļ��У�����FCBList
	MyFCBHead* readPathFCBFile(string filePath);
	//����input�еĵ�һ���ļ���
	string findFirstFileName(string*);
	//��blockNum������д��input���������ݣ�ʹ�ú�Ӧ��ʹ��readFCBBlocks��ˢ��input������
	void writeFCBBlocks(MyFCBHead*);
	//��ȡblockNum�е�Ŀ¼����
	MyFCBHead* readFCBBlocks(int);

	//��¼���ɹ��򷵻�true������userName��userId��ʧ�ܷ���false
	bool commandLogin();
	//��ʾ�����Ѵ������û���������id
	void commandShowUser();
	//���Դ���һ�����û�
	void commandCreateUser();
	//����ɾ��һ�������û�
	void commandDeleteUser();
	//�������Ϊinput���û����������򷵻�nullptr
	User* checkUserName(string);
	//���ҳ�Ϊ��ʹ�õ�id����User������ʧ�ܷ���nullptr
	User* getNewUser();
	/*										*/
	//����input�е�num���ַ���
	void findString(int);

	//��blockNum��FCBNumλ��д��FCB�����Ϊ0~64
	void writeFCB(int, int, MyFCB*);
	//����blockNum��FCBNumλ�õ�FCB����
	MyFCB* readFCB(int, int);
	//ɾ����ǰ�鼰֮���FCB�飬��ɾ����һ����ע�����FCB��storageBlock
	void deleteFCBBlock(int);
	//����block�����һ����
	void setNextFCBBlock(int, int);
	//��ȡblock�����һ���飬Ϊ0�򲻴���
	int getNextFCBBlock(int);
	//���ü�¼blockNum��Ŀ¼�ĸ���
	void setPathParentBlock(int, int);
	//��ȡ��¼blockNum��Ŀ¼�ĸ���
	int getPathParentBlock(int blockNum);

	//ɾ��ָ��id��user������
	void deleteUser(int);
	//��ȡָ��id��Ӧλ�õ�user
	User* readUser(int);
	//�������userд�뵽id��Ӧ��λ��
	void writeUser(User*);
	/*										*/
	//��ȡ���뷵�ص�output�У�����ֵΪ�ַ����ܳ���
	void getCommand();

	//���ҵ���һ��δʹ�õĿ飬��ʼ�������ؿ��
	int findNewBlock();
	//д�뵽ָ����
	void writeBlock(char*, int);
	//��ȡָ���飬ע���ͷ��ڴ�
	char* readBlock(int);
	//��ձ��ΪblockNum�Ŀ�
	void clearBlock(int);
	//��blockNum���posNumλ��д�볤��Ϊlen��input,posNum+len���ɳ���BLOCK_SIZE
	void writeLine(char*, int, int, int);
	//��ȡblockNum���posNumλ�ó���Ϊlen�����ݷ���,posNum+len���ɳ���BLOCK_SIZE
	char* readLine(int, int, int);
	//��ȡ�Ѵ����ܿ��������洢��1�ſ�parentBlockλ��
	int getSumBlockNum();
	//�����Ѵ����ܿ��������洢��1�ſ�parentBlockλ��
	void setSumBlockNum(int);
	//���ñ��ΪblockNum�Ŀ��ʹ��״̬Ϊstatus
	void setBlockStage(int, bool);
	//��ȡ���ΪblockNum�Ŀ��ʹ��״̬
	bool getBlockStage(int);
	/*										*/
	//�ļ���
	fstream ioFile;
	//���������
	string input;

	//char����ת��Ϊstring
	string charToString(char*);
	//�����ļ�����ʼ����ʽ
	void tryOpenFile();
	//��ģ������ļ��д����µĲ���
	int createBlock();
	//д��input��blockNum���posNumλ��
	void writeStorage(unsigned char, int, int);
	//��ȡblockNum���posNumλ�����ݷ���
	unsigned char readStorage(int, int);
};