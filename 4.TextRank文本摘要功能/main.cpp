#include <cstdlib>
#include <iostream>
#include <sstream>
using namespace std;

void usage(char** argv)
{
	cout << endl;
	cout << "USAGE:" << endl;
	cout << argv[0] <<" [要提取的文本文件] [需要提取的摘要句子个数]" << endl;
	cout << "exp : ./main InstallPackage/test/doc/01.txt 3" << endl;
	cout << endl;
}

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		usage(argv);
		return 1;
	}
	
	cout << "文本文件：" << argv[1] << endl;
	cout << "摘要个数：" << argv[2] << endl;
	
	stringstream ss;
	ss << "python DigestExtract.py " << argv[1] << " " << argv[2];
	
	string strcmd(ss.str());
	int status = system(strcmd.c_str());
	
	if (status != 0)
	{
		cout << "提取摘要失败\n" << endl;
		usage(argv);
		return 1;
	}
	
	cout << "\nfinished" << endl;
	
	return 0;
}
