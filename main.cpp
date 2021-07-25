#include <iostream>
#include <iomanip>
#include <map>
#include <fstream>
#include "flow.h"
#include "BaseClass.h"
extern double reads_runs = 0;
vector<string> patterns;
void showpos(i64 *pos, i64 num);
void showpos(vector<string> pos);
int main(int argc, char *argv[])
{
	FTcomp ft;
	globalArgs.MODE = MODE::build;
	globalArgs.FILENAME = "";
	int ch;
	int longIndex = 0;
	while ((ch = getopt_long(argc, argv, optString, longOpts, &longIndex)) != -1)
	{
		switch (ch)
		{
		case 'z':
			// cout << string(optarg) << endl;
			if (string(optarg).compare("lr") == 0)
			{
				globalArgs.MODE = MODE::lr;
			}
			else if (string(optarg).compare("cr") == 0)
			{ //build compress
				globalArgs.MODE = MODE::cr;
			}
			else if (string(optarg).compare("er") == 0)
			{ //build compress
				globalArgs.MODE = MODE::er;
			}
			else if (string(optarg).compare("efq") == 0)
			{ //build compress
				globalArgs.MODE = MODE::efq;
			}
			else if (string(optarg).compare("build") == 0)
			{ //build compress
				globalArgs.MODE = MODE::build;
			}
			else if (string(optarg).compare("help") == 0)
			{
				globalArgs.MODE = MODE::help;
			}
			else
			{
				cout << __LINE__ << endl;
				globalArgs.MODE = MODE::help;
			}
			break;
		case 'f':
			globalArgs.FILENAME = string(optarg);
			break;
		}
	}
	string opt = (std ::string)argv[1];
	switch (globalArgs.MODE)
	{
	case MODE::build :
		{cout << "mode build start" << endl;
		string inputfilename = globalArgs.FILENAME;
		string savePath = inputfilename.substr(0, inputfilename.find_last_of("/\\") + 1);
		cout << savePath << endl;
		struct timeval start_b, end_b;
		gettimeofday(&start_b, NULL);
		ft.compFastqFile(inputfilename, savePath);
		gettimeofday(&end_b, NULL);
		cout << "-----------build Done!-------------\n";
		double timeUsed = (((long)end_b.tv_sec) * 1000 + ((long)end_b.tv_usec) / 1000 - ((long)start_b.tv_sec) * 1000 + ((long)start_b.tv_usec) / 1000) / 1000.0;
		break;}
	case MODE::cr :
		{	
			cout << "CountR" << endl;
			FTcomp ft;
			string filepath = globalArgs.FILENAME;
			string savePath = filepath.substr(0, filepath.find_last_of("/\\") + 1);
			ft.LoadIndexFromCompressedFile(savePath, savePath + "packedDNA.txt.CMP.txt", savePath + "preQVfile.txt.CMP.txt", savePath);
			while(1){
				string str;
				cout<<"input pattern:"<<endl;
				cin>>str;
				if(str =="exit")
					break;
				cout << str<<"  occ:"<< ft.GetDnaPatternCount(str) << endl;
			}

		break;}
	case MODE::lr :
		{
			cout << "LocatR" << endl;
			FTcomp ft;
			string filepath = globalArgs.FILENAME;
			string savePath = filepath.substr(0, filepath.find_last_of("/\\") + 1);
			ft.LoadIndexFromCompressedFile(savePath, savePath + "packedDNA.txt.CMP.txt", savePath + "preQVfile.txt.CMP.txt", savePath);
			i64 num = 0;
			while (1)
			{
				string str;
				cout << "input pattern:" << endl;
				cin >> str;
				if (str == "exit")
					break;
				i64* pos = ft.GetDnaPatternLine(str, num);
				cout<< str << "  occ:" <<	num <<endl;
				showpos(pos,num);
			}
		break;
		}
	case MODE::er :
		{
			cout << "ExtractR" << endl;
			FTcomp ft;
			string filepath = globalArgs.FILENAME;
			string savePath = filepath.substr(0, filepath.find_last_of("/\\") + 1);
			ft.LoadIndexFromCompressedFile(savePath, savePath + "packedDNA.txt.CMP.txt", savePath + "preQVfile.txt.CMP.txt", savePath);
			i64 num = 0;
			while (1)
			{
				string str;
				cout << "input lineno:" << endl;
				cin >> str;
				if (str == "exit")
					break;
				if (atoi(str.c_str()) == 0&&str!="0")
				{
					cout << "input lineno:"<< endl;
					continue;
				}
				u64 poss = atoi(str.c_str());
				cout << poss << " line of DNA : " <<ft.DNAextract(poss) << endl;
			}
			break;
		}
	case MODE::efq:
		{
			cout << "extractFASTQ" << endl;
			FTcomp ft;
			string filepath = globalArgs.FILENAME;
			string savePath = filepath.substr(0, filepath.find_last_of("/\\") + 1);
			ft.LoadIndexFromCompressedFile(savePath, savePath + "packedDNA.txt.CMP.txt", savePath + "preQVfile.txt.CMP.txt", savePath);
			i64 num = 0;
			while (1)
			{
				string str;
				cout << "input pattern:" << endl;
				cin >> str;
				if (str == "exit")
					break;
				vector<string> vecstr;
				ft.GetFASTQ(str,vecstr);
				// cout << str << "  occ:" << num << endl;
				showpos(vecstr);
			}
			break;
		}
	case MODE::help:
	{
		cout << "--------------------------------------------------------------------------------------" << endl;
		cout<< "first you need to compress FASTQ,then build index of the FASTQ FILE;finally you can use the index functions."<<endl;
		cout << "The flowing commands are supported " << endl;
		cout <<"compress and build index FASTQ:  CIndex -z build -f fastqpath" << endl;
		cout << "count DNA: CIndex -z cr ,then input the pattern you want to count" << endl;
		cout << "locate DNA: CIndex -z lr ,then input the pattern you want to locate" << endl;
		cout << "extract DNA: CIndex -z er ,then input the lineno you want to extract" << endl;
		cout << "extract FASTQ: CIndex -z efq ,then input the pattern you want to extract" << endl;
		break;
	}
	}
	return 0;
}
void showpos(i64 *pos, i64 num)
{
	cout << "occs:" << num << endl;
	for (long int i = 0; i < num; i++)
	{
		cout << pos[i] << endl;
		if ((i + 1) % 20 == 0)
		{
			char command;
			cout << "-----------------more---------------------";
			system("stty raw");
			command = getchar();
			cout << endl
				 << '\r';
			system("stty cooked");
			if (command == 27)
			{
				cout << endl;
				return;
			}
		}
	}
}
void showpos(vector<string> pos)
{
	cout << "occs:" << pos.size() << endl;
	for (long int i = 0; i < pos.size(); i++)
	{
		cout << pos[i] << endl;
		if ((i + 1) % 20 == 0)
		{
			char command;
			cout << "-----------------more---------------------";
			system("stty raw");
			command = getchar();
			cout << endl
				 << '\r';
			system("stty cooked");
			if (command == 27)
			{
				cout << endl;
				return;
			}
		}
	}
}
