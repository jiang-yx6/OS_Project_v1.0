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
			if (command=="mkdir")
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
		fileInitialize();
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
}

void File::fileInitialize()
{
	addBlock(0);

	memPos = 1;
	writeMem(1);//���0�ſ���ʹ��
}

void File::addBlock(int blockNum)
{
	memPos = BLOCK_SIZE * blockNum;
	placeHold(BLOCK_SIZE);
}

void File::placeHold(int num)
{
	while (num > 0)
	{
		writeMem(0);
		num--;
	}
}

void File::writeMem(int num)
{
	//0~255
	//ֱ����ʾģ���ڴ�������
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
		default:
			num += (int)tmp[0] - 48;
			break;
		}
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
	memPos = blockNum * BLOCK_SIZE + 2;
	int tmp,nextBlock;
	while(1){
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
	}

}
