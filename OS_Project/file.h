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
* �����飺ǰ�ĸ��ֽڿճ�
* �ļ��飺ǰ�����ֽڿճ�
*/

/*���ļ������飺
* ���ּ�
* ��һ����Ϊ��ʹ�÷���
* �ڶ�����Ϊ������
* �����ĸ���Ϊ�ļ��ܳ���
*/

//ÿ��4KB---��ͬʱ����2008�飬���ļ����Լ8MB
//Ŀ��� 2�ֽ�--------65535�飬�ܴ�СΪ256MB

class File
{
public:
	File();
private:
	ProcessManager pm;

	//��ѡ��������λ�Ѽ���FCB�飬���ظÿ���list��λ��
	//int blockType;//��¼��ǰ���ص�block������1��Ŀ¼�飬0���ļ���
	//��һ����λFCBList���ļ���--locateBlock��commandDeletePath
	//����ַ����ֶ���Ŀ
	//�޸�����ʹ��cin�����
	//���ֹɾ���򿪵��ļ�
public:
	//��¼����
	void loginIn();

	//�ļ����ƽ���
	void fileControl();
	/*										*/	
	//����Ŀ¼����
	void commandChangePath(string);
	//��ʾinput�µ�Ŀ¼�е��ļ����ÿմ���Ϊ��ǰ�ļ�·��
	void commandShowPathFile(string);
	//�ڵ�ǰĿ¼�´���һ���ļ���,��������д��FCBͷ��������ռ�
	void commandCreatePath(string);
	//�����ļ�
	void commandCreateFile(string);
	//ɾ��ָ����Ŀ¼����Ŀ¼����Ϊ��Ŀ¼
	void commandDeletePath(string);
	//ɾ��ָ���ļ�
	void commandDeleteFile(string);
	//�����ļ�Ȩ��
	void commandChangePermission(string);
	//���ļ�д������
	void commandWriteFile(string);
	//����ļ�����
	void commandShowFile(string);
	//ģ��VIM����
	void commandVim(string);

	std::string getSecondPart(const std::string& input);
private:
	//��¼��·��
	string path;//-------------------------------���ܻ������
	//��¼��ǰ·����FCB����
	MyFCBHead* FCBList;
	//��¼��ǰ�û���
	string userName;
	//��¼��ǰ�û�id
	int userId;

	//������Ŀ¼���ļ���
	void loadMainPath();
	//��ȡ·��ָ���FCB�飬���ص�firstFCB�У����������ڿ鵽currentBlock�У�����nullptrΪ��ȡʧ�ܣ���Ŀ¼��FCB����Ӧ��ȡ
	MyFCBHead* readPathFCB(string);
	//��ȡ·��ָ����ļ��У�����FCBList
	MyFCBHead* readPathFCBFile(string);
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
	//����input�е�num���ַ�����д�뵽command��
	string findString(int,string);

	//�����ļ��������ڵĿ���Լ��ļ�ָ����ʼλ���ַ��ı�����ȡ���ȣ����ض�ȡ�Ľ����������nullptr����������'\0'����
	char* readFileLine(int, int, int);
	//��ʾ�ļ����ݵ���Ļ��
	void showFile(int blockNum);
	//���ļ���ָ��λ���������
	void addFileData(int,int,char);
	//ɾ���ļ���ָ��λ�õ�����
	void delFileData(int,int);
	//�����ļ��ܳ��ȼ�1�������½��¿�
	void addFileTotalLen(int);
	//�����ļ��ܳ��ȼ�1��ͬʱɾ������Ŀ�
	void subFileTotalLen(int);
	//��ȡ�����ļ���ָ��λ�õ�����
	unsigned char readFile(int, int);
	//���ļ���ָ��λ��д������
	void writeFile(int,int,unsigned char);
	//��ȡ�ļ��ܳ���
	int getFileTotalLen(int);
	//�����ļ��ܳ���
	void setFileTotalLen(int,int);
	//���ļ������ӿ�
	void addBlock(int);
	//ɾ���ļ����Ŀ�
	void delBlock(int);
	//�����ļ�·����¼blockNum���ļ��ĸ���
	void setFileParentBlock(int,int);
	//��ȡ�ļ�·����¼blockNum���ļ��ĸ���
	int getFileParentBlock(int);
	//��λpos���ļ��ж�Ӧ�Ŀ�
	int locateFileBlock(int,int);
	//��λpos���ļ��ж�Ӧ�Ŀ��е�λ�ã���Ŵ�0��ʼ
	int locateFilePos(int,int);

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
	//����·����¼blockNum��Ŀ¼�ĸ���
	void setPathParentBlock(int, int);
	//��ȡ·����¼blockNum��Ŀ¼�ĸ���
	int getPathParentBlock(int);

	//ɾ��ָ��id��user������
	void deleteUser(int);
	//��ȡָ��id��Ӧλ�õ�user
	User* readUser(int);
	//�������userд�뵽id��Ӧ��λ��
	void writeUser(User*);
	/*										*/
	//��ȡ���뷵�ص�output�У�����ֵΪ�ַ����ܳ���
	string getCommand();

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