#include "file.h"

//#define DEBUG

void File::fileControl()
{
	system("cls");

	bool cycleFlag = true;
	cin.get();//�����������δ����Ļ��з�
	loadPath();

	while (cycleFlag) {
		cout << "[" << path << "]>";
		getline(cin, input);//��������ȡ�����ַ�������ʱ���ܻ��������
		inputPos = input.begin();
		findFirstCommand();
		if (!command.empty())
		{

#ifndef DEBUG
			cout << "���������Ϊ:" << command << endl;
#endif // !DEBUG

			if (command=="cd")//·�����~�Ŵ������ַ��ʹ��\�ָ��ļ�
			{
				commandChangePath();
				continue;
			}
			if (command=="ls")
			{
				//commandShowPathFile();
			}
			if (command=="mkdir")//����ļ�������
			{
				commandCreatePath();
				continue;
			}

			//list<MyFCB>::iterator it = FCBList.begin();

		}
	}
	
}

void File::findFirstCommand()
{
	int pos;
	string tmp = "";
	//string::iterator pos = input.begin();
	while (inputPos!=input.end()&&*inputPos==' ')
	{
		inputPos++;
	}//�����ո�
	for (;inputPos != input.end()&&*inputPos!=' '; inputPos++)
	{
		tmp += *inputPos;
	}
	command = tmp;

//	pos = input.find(" ");//δ�ҵ��᷵��-1
}

void File::loadPath()
{
	//�ļ�����Ϊstorage
	ioFile.open("storage.txt");
	if (!ioFile.is_open())//���ļ������´���
	{
		ioFile.open("storage.txt",ios::out);//fstream����ֻдģʽ�����ļ�
		ioFile.close();
		ioFile.open("storage.txt");
		newBlock();
		cout << "�ļ������ɹ�" << endl;
		ioFile.close();//�����ļ�
		ioFile.open("storage.txt");
	}

	command = "~";
	locateBlock();

	//char reader[1024];//seekԽ�粻����ʾ�������ȡ��������reader��ԭ�е����ݲ���ı�
}

void File::commandChangePath()
{
	command = "";
	findFirstCommand();
	locateBlock();
}

void File::commandCreatePath()
{
	MyFCB tmp;
	string strTmp;
	command = "";
	findFirstCommand();
	if (command.size() > 49)
	{
		cout << "�ļ������ȹ���" << endl;
	}
	for (list<MyFCB>::const_iterator it=FCBList.begin(); it != FCBList.end(); it++)
	{
		tmp=*it;
		strTmp = "";
		for (size_t i = 0; i < 49&& tmp.name[i]!=(char)0; i++)
		{
			strTmp += tmp.name[i];
		}
		if (strTmp==command)
		{
			cout << "���ļ����Ѵ���" << endl;
			return;
		}
	}

	FCBList.push_back(MyFCB(command,192));

	writeAllFCB();

}

//void File::fileInitialize()
//{
//	addBlock(0);
//
//	memPos = 1;
//	writeMem(1);//���0�ſ���ʹ��
//}

void File::clearBlock(int blockNum)
{
	memPos = BLOCK_SIZE * blockNum;
	placeHold(BLOCK_SIZE);
	memPos = blockNum * BLOCK_SIZE+1;
	writeMem(1);
}

void File::placeHold(int num)
{
	while (num > 0)
	{
		writeMem(0);
		num--;
	}
}

void File::writeMem(char ch)
{
	//0~255
	//ֱ����ʾģ���ڴ�������
	unsigned char num = ch;
	ioFile.seekp(memPos * 3);
	int tmp1, tmp2;
	tmp1 = num / 16;
	tmp2 = num % 16;
	num = 2;
	while(num){
		switch (tmp1)
		{
		case 10:
			ioFile.write("A", 1);
			break;
		case 11:
			ioFile.write("B", 1);
			break;
		case 12:
			ioFile.write("C", 1);
			break;
		case 13:
			ioFile.write("D", 1);
			break;
		case 14:
			ioFile.write("E", 1);
			break;
		case 15:
			ioFile.write("F", 1);
			break;
		default:
			ioFile.write(to_string(tmp1).c_str(), 1);
			break;
		}
		tmp1 = tmp2;
		num--;
	}
	ioFile.write(" ", 1);

	//ʹ��16����ascii��鿴�����鿴�ڴ�����
	/*ioFile.seekp(memPos);
	ioFile.put((char)num);*/

	memPos++;
}

int File::readMem()
{
	//0~255
	//ֱ����ʾģ���ڴ�������
	ioFile.seekg(memPos * 3);
	char tmp[1];
	int num=0;
	tmp[0] = 0;
	ioFile.read(tmp, 1);
	while (tmp[0]!=' ') {
		num *= 16;
		switch (tmp[0])
		{
		case 'A':
			num += 10;
			break;
		case 'B':
			num += 11;
			break;
		case 'C':
			num += 12;
			break;
		case 'D':
			num += 13;
			break;
		case 'E':
			num += 14;
			break;
		case 'F':
			num += 15;
			break;
		case 0:
			return -1;
		default:
			num += (int)tmp[0] - 48;
			break;
		}
		tmp[0] = 0;
		ioFile.read(tmp, 1);
	}
	//ʹ��16����ascii��鿴�����鿴�ڴ�����
	/*ioFile.seekg(memPos);
	num = (int)ioFile.get();*/

	memPos++;
	return num;
}

int File::locateBlock()
{
	int cutPos=0,cutEnd;
	string tmp;
	cutEnd = command.find('//');
	tmp = command.substr(cutPos,cutEnd);//�ڶ�������Ϊ-1��������β����Ч��������
	if (strcmp(tmp.c_str(), "~")==0)
	{
		//�����Ӹ�Ŀ¼��
		loadFCB(0);
	}
	return 0;
}

void File::loadFCB(int blockNum)
{
	FCBList.clear();
	MyFCB tmpFCB;
	int tmp,nextBlock;
	while(1){
		memPos = blockNum * BLOCK_SIZE + 2;
		nextBlock = readMem();
		nextBlock *= 256;
		nextBlock += readMem();
		for (size_t i = 1; i < 4096 / 64; i++)
		{
			memPos = blockNum * BLOCK_SIZE + i * 64;
			tmp = readMem();
			if ((tmp & 128) == 0)
			{
				continue;
			}
			tmpFCB.dataFlag = tmp;
			for (size_t i = 0; i < 49; i++)
			{
				tmpFCB.name[i] = (char)readMem();
			}
			for (size_t i = 0; i < 6; i++)
			{
				tmpFCB.createTime[i] = (char)readMem();
			}
			for (size_t i = 0; i < 6; i++)
			{
				tmpFCB.changeTime[i] = (char)readMem();
			}
			tmpFCB.storageBlock = readMem();
			tmpFCB.storageBlock *= 256;
			tmpFCB.storageBlock += readMem();
			FCBList.push_back(tmpFCB);
		}
		if (nextBlock == 0)
		{
			break;
		}
		blockNum = nextBlock;
	}

}

void File::writeAllFCB()
{
	int block,nextBlock;
	block=locateBlock();
	memPos = block * BLOCK_SIZE+2;
	nextBlock = readMem() * 256 + readMem();

	clearBlock(block);
	memPos = block * BLOCK_SIZE + 64;
	for (list<MyFCB>::iterator it = FCBList.begin(); it !=FCBList.end() ; it++)
	{
		if (memPos==(block+1)*BLOCK_SIZE)//ȡ��д��
		{
			//������һ��/�����¿�
			if (nextBlock!=0)
			{
				block = nextBlock;
				memPos = block * BLOCK_SIZE + 2;
				nextBlock = readMem() * 256 + readMem();
				clearBlock(block);
				memPos = block * BLOCK_SIZE + 64;
			}
			else
			{
				nextBlock = newBlock();
				memPos = block * BLOCK_SIZE + 2;
				writeMem(nextBlock / 256);
				writeMem(nextBlock % 256);
				block = nextBlock;
				nextBlock = 0;
				memPos = block * BLOCK_SIZE + 64;
			}
		}

		writeFCB(it);
	}

	//�ж�ԭ�п��Ƿ����ʹ�ã�δʹ�õ�ɾ��
	if (nextBlock!=0)
	{
		memPos = block * BLOCK_SIZE + 2;
		writeMem(0);
		writeMem(0);
		while (nextBlock != 0)
		{
			block = nextBlock;
			memPos = block * BLOCK_SIZE;
			writeMem(0);
			writeMem(0);
			memPos = block * BLOCK_SIZE + 2;
			nextBlock = readMem() * 256 + readMem();
		}
	}

}

int File::newBlock()
{
	int block = -1;
	int flag=1;
	//�����ȡ����᷵��-1
	while (flag)
	{
		block++;
		memPos = block * BLOCK_SIZE;
		flag = readMem() * 256 + readMem();
		if (flag<0) break;//����ȡʧ�ܣ������¿�---------��ȡ����������´��ļ�
	}
	ioFile.close();
	ioFile.open("storage.txt");
	clearBlock(block);

	return block;
}

void File::writeFCB(list<MyFCB>::iterator it)
{
	MyFCB tmp = *it;
	writeMem(tmp.dataFlag);
	for (size_t i = 0; i < 49; i++) writeMem(tmp.name[i]);
	for (size_t i = 0; i < 6; i++) writeMem(tmp.createTime[i]);
	for (size_t i = 0; i < 6; i++) writeMem(tmp.changeTime[i]);
	writeMem(tmp.storageBlock / 256);
	writeMem(tmp.storageBlock % 256);
}
