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

		}
		else
		{

#ifndef DEBUG
			cout << "����" << endl;
#endif // !DEBUG

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
	ioFile.open("storage.txt",ios::binary|ios::in|ios::out);
	if (!ioFile.is_open())
	{
		ioFile.open("storage.txt",ios::out);//fstream����ֻдģʽ�����ļ�
		ioFile.close();
		ioFile.open("storage.txt", ios::binary | ios::in | ios::out);
		fileInitialize();
		cout << "�ļ������ɹ�" << endl;
	}



	//char reader[1024];//seekԽ�粻����ʾ�������ȡ��������reader��ԭ�е����ݲ���ı�
	/*if (ioFile.is_open())
	{
		cout << "";
	}*/
}

void File::commandChangePath()
{
}

void File::fileInitialize()
{
	memPos = 0;
	addClock();


}

void File::addClock()
{
	placeHold(CLOCK_SIZE);

	cout << "ָ��λ�ü��" << endl;
}

void File::placeHold(int num)
{
	while (num > 0)
	{
		writeMem(0);
		memPos++;
		num--;
	}
}

void File::writeMem(int num)
{
	//0~255
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
}
