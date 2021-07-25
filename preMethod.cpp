#include"preMethod.h"
extern int seqlength ;
//preprpcess DNA
int charMap[128] = { 255 };
unsigned char table[5] = { 0 };
map<string, int> dict;
map<int, string> dict2;
map<string, int> countT;
void initialize(int *charMap, map<string, int> &dict, map<string, int>&countT)
{
	charMap['A'] = 0;
	charMap['C'] = 1;
	charMap['G'] = 2;
	charMap['N'] = 3;
	charMap['T'] = 4;

	unsigned char table[5] = { 0 };
	table[0] = 'A';
	table[1] = 'C';
	table[2] = 'G';
	table[3] = 'N';
	table[4] = 'T';
	int index = 1;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			for (int k = 0; k < 5; k++)
			{
				string str = "";
				str += table[i];
				str += table[j];
				str += table[k];
				if (index == 10)
				{
					dict[str] = 126; index++;
				}
				else if (index == 13)
				{
					dict[str] = 127; index++;
				}
				else
				{
					dict[str] = index++;
				}
				countT[str] = 0;
			}
		}
	}

	return;
}
void initialize2(int *charMap, unsigned char*table, map<int, string> &dict2)
{
	charMap['A'] = 0;
	charMap['C'] = 1;
	charMap['G'] = 2;
	charMap['N'] = 3;
	charMap['T'] = 4;


	table[0] = 'A';
	table[1] = 'C';
	table[2] = 'G';
	table[3] = 'N';
	table[4] = 'T';
	int index = 1;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			for (int k = 0; k < 5; k++)
			{
				string str = "";
				str += table[i];
				str += table[j];
				str += table[k];
				if (index == 10)
				{
					dict2[126] = str; index++;
				}
				else if (index == 13)
				{
					dict2[127] = str; index++;
				}
				else
				{
					dict2[index++] = str;
				}
				countT[str] = 0;

			}
		}
	}
	return;
}
void showcountT()
{
	cout << "show countT:\n";
	unsigned char table[5] = { 0 };
	table[0] = 'A';
	table[1] = 'C';
	table[2] = 'G';
	table[3] = 'N';
	table[4] = 'T';
	int index = 0;
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			for (int k = 0; k < 5; k++)
			{
				string str = "";
				str += table[i];
				str += table[j];
				str += table[k];
				//dict[str] = index++;
				cout << str << ": " << countT[str] << endl;
			}
		}
	}
	return;
}
void tripleBase(string str, string &ans)
{
	unsigned char code = 0;
	if (str.length() == 3)
	{
		code = dict[str];
		if (code == 10 | code == 13 | code < 0)
		{
			cout << "triplebase error:index valid: code = " << (int)code << endl;
			exit(-1);
		}
		countT[str]++;
		ans += code;
	}
	else
	{
		cout << "triple != 3\n";
		exit(-1);
	}
	//test
	int num = code;
	if (num >= 128 || num < 0)
		cout << "triple error:" << code << endl;
	return;
}
void run_length(string temp, string &ans)
{
	unsigned char code = 128;
	string str = "";
	for (int i = 0; i < temp.length() - 1; i++)
		str += temp[i];
	if (temp.length() == 4)
		tripleBase(str, ans);
	else if (temp.length() > 4)
	{
		int len = temp.length() - 5;
		unsigned char c = str[0];
		int index = charMap[c];
		code = 128;
		code += (index << 4);
		code += len;
		ans += code;
	}
	else
	{
		cout << "run-length < 4\n";
		exit(-1);
	}
	//test
	int num = code;
	if (num < 128 || num < 0)
		cout << "run-length error:" << num << endl;
	return;
}
bool judge0to3(unsigned char ch)
{
	if(ch>= '0' && ch<='3')
		return true;
	else
	{
		return false;
	}
}
void PreProceDNA(unsigned char *line, unsigned long long & lineLength)
{
	unsigned char maps[] = {'A','C','G','T'};
	if (judge0to3(line[1]) || judge0to3(line[2])|| judge0to3(line[3])){
		unsigned long long zz = 0;
		for (unsigned long long i = 0;i<lineLength;i++){
			if(line[i]=='G')
				zz++;
			else if(judge0to3(line[i]))
				line[i-zz] = maps[line[i]-'0'];
			else
			{
				line[i - zz] = line[i];
			}
		}
		lineLength -=zz;
	}

	else
	{
		return;
	}
}
void PackDNA(int seqlength, string inputfile, string outputfile)
{
	// return;
	initialize(charMap, dict, countT);
	unsigned char *line = NULL;
	unsigned long long lineLength = 0;
	line = Getfile(inputfile.c_str(), lineLength);
	PreProceDNA(line,lineLength);
	string temp = "";
	string ans = "";
	int len = 1;
	temp = line[0];
	string tail = "";

	for (unsigned long long int i = 1; i < lineLength; i++)
	{
		if (line[i] < 33)
		{
			continue;
		}
		temp += line[i];
		len++;
		if (temp.length() == 3)
		{
			if (temp[2] != temp[1] || temp[2] != temp[0])
			{
				tripleBase(temp, ans); temp = ""; len = 0; continue;
			}
			else continue;
		}
		else if ((len > 3 && temp[len - 1] != temp[len - 2]) || len == 20)
		{
			run_length(temp, ans);
			temp = line[i]; len = 1; continue;
		}
		else continue;
	}

	delete[] line;

	bool flag = 1;
	if (temp != "")
	{
		if (temp.length() > 3)
		{
			run_length(temp, ans);
			flag = 0;
			tail = temp[temp.length() - 1];
		}
		else if (temp.length() == 3)
		{
			tripleBase(temp, ans);
		}
		else
		{
			flag = 0;
			tail = temp;
		}
	}

	ofstream fout;
	fout.open(outputfile, ios_base::out | ios_base::trunc | ios_base :: binary);
	if (!fout.is_open()){ cout << "can't write the file.\n"; exit(-1); }
	if (flag)
	{
		fout << '1' << endl;
		//fout << ans;
		cout << "Pack:ans length =  " << ans.length() << endl;
	}
	else
	{
		fout << tail << endl;
		//fout << ans;
		cout << "Pack:ans length =  " << ans.length() << endl;
	}
	// write seqlength
	fout << seqlength << endl;

	//test
	for (u32 i = 0; i < ans.length(); i++)
	{
		unsigned char c = ans[i];
		fout << c;
	}
	fout.close();
	//remove
	// if(remove(inputfile.c_str()) == -1){cout << "remove " << inputfile << " failed.";}
	return;
}
unsigned char* Getfile(const char *filename, unsigned long long&linelength)
{

	FILE*fp = fopen(filename, "rb"); //get the pointer of file;
	if (fp == NULL) {
		cout << "can not find the file!" << endl;
		exit(0);
	}
	fseek(fp, 0, SEEK_END);//put the pointer on the file end ; 0 represents the pointer overloop 0 Byte from SEEK_END;
	unsigned long long n = ftell(fp);//return the overloop;
	linelength = n;
	fseek(fp, 0, SEEK_SET);
	unsigned char*T = new unsigned char[n];
	unsigned long long int e = 0;
	int num = 0;
	e = fread(T, sizeof(unsigned char), n, fp);
	//std::cout << "n = " << n << std::endl;

	if (e != n)
	{
		cout << "Getfile failed!" << endl;
		exit(-1);
	}
	else
	{
		cout << "Getfile success!" << endl;
	}
	fclose(fp);
	return T;
}
void PreunProceDNA(unsigned char *line, unsigned long long &lineLength)
{
	unsigned char maps[] = {'A', 'C', 'G', 'T'};
	if (judge0to3(line[1]) || judge0to3(line[2]) || judge0to3(line[3]))
	{
		unsigned long long zz = 0;
		for (unsigned long long i = 0; i < lineLength; i++)
		{
			if (line[i] == 'G')
				zz++;
			else if (judge0to3(line[i]))
				line[i - zz] = maps[line[i] - '0'];
			else
			{
				line[i - zz] = line[i];
			}
		}
		lineLength -= zz;
	}

	else
	{
		return;
	}
}
void unpackDNA(string inputfile, string outputfile)
{
	initialize2(charMap, table, dict2);
	string tail = "";
	ifstream fin;
	fin.open(inputfile, ios_base::in | ios_base::binary);
	if (!fin.is_open()){ cout << "Can't open file.\n"; exit(-1); }
	getline(fin, tail);

	string temp = "";
	getline(fin, temp);
	seqlength = atoi(temp.c_str());
	getline(fin,temp);
	fin.close();

	bool runlengthtail = 0;
	if (tail == "1")
		runlengthtail = 1;

	string ans;
	ans = "";
	unsigned long long length = temp.length();

	//unsigned char * temp = Getfile(inputfile.c_str(), length);
	//for (unsigned long long i = tail.length() + 1; i < length; i++)

	for (unsigned long long i = 0; i < length; i++)
	{
		unsigned char code = temp[i];
		if (code >= 128)//run-length
		{
			int index = code & 0x70;
			index = index >> 4;
			unsigned char ch = table[index];
			int len = code & 0x0f;
			len += 4;
			int k = 0;
			while (k < len)
			{
				ans += ch;
				k++;
			}
		}
		else //triple
		{
			int index = code & 0x7f;
			if (index == 13 || index == 10)
			{
				cout << "index error:" << index << endl;
				exit(-1);
			}
			string str = dict2[index];
			ans += str;
		}
	}

	//triple tail
	if (!runlengthtail)
	{
		ans += tail;
	}
	ofstream fout;
	fout.open(outputfile, ios_base::out | ios_base::trunc);
	cout << "unpacked ans length = " << ans.length() << endl;
	//int seqLen = ans.length() / seqNum;

	int k = 0;
	string seq = "";
	for (unsigned long long int i = 0; i < ans.length(); i++)
	{
		if (k == seqlength - 1)
		{
			seq += ans[i];
			fout << seq << endl;
			k = 0;
			seq = "";

		}
		else
		{
			seq += ans[i];
			k++;
		}
	}

	fout << seq << endl;
	fout.close();
	if(remove(inputfile.c_str()) == -1){cout << "remove " << inputfile << " failed.";}
	return;
}
//QV preprocess
string Bytetwo(string str)
{
	string ans = "";
	unsigned char pre = str[0];
	// str =str+ "\n";
	unsigned int length = 1;
	for (int i = 1; i < str.length(); i++)
	{
		if (str[i] == pre)
		{
			length++;
		}
		else
		{
		/////////old liuxi has bug
		// 	if (length == 1)
		// 	{
		// 		ans += pre;
		// 		pre = str[i];
		// 	}
		// 	else
		// 	{
		// 		ans += pre;
		// 		unsigned char len = 128;
		// 		len += length;
		// 		ans += len;
		// 		length = 1;
		// 		pre = str[i];
		// 	}
		// }
		//////////////////////
			if (length == 1)
			{
				ans += pre;
				pre = str[i];
			}
			else
			{
				ans += pre;
				// unsigned char len = 128;
				while(length >127){
					length-=127;
					// len += 127;
					ans += (unsigned char)(128+127);
					ans += pre;
				}
				// len += length;
				ans += (unsigned char)(128 + length);
				length = 1;
				pre = str[i];
			}
		}
	}
	//////////////////////// old liuxi has bug
	// if (length == 1)
	// 	ans += pre;
	// else
	// {
	// 	unsigned char len = 128;
	// 	ans += pre;
	// 	len += length;
	// 	ans += len;
	// 	length = 1;
	// }
	//////////////////////////
	if (length == 1)
		ans += pre;
	else
	{
		// unsigned char len = 128;
		ans += pre;
		// unsigned char len = 128;
		while (length > 127)
		{
			length -= 127;
			// len += 127;
			ans += (unsigned char)(128 + 127);
			ans += pre;
		}
		// len += length;
		ans += (unsigned char)(128 + length);
		length = 1;
	}
	auto pospos = ans.find('\0');
	if (pospos != string::npos)
		{
			cout << pospos<<endl<< str<<endl<<ans << endl;
			for (int zz = 0; zz < ans.length(); zz++)
				cout << (int)ans[zz] << ",";
			cout<<endl;
		}
	return ans;
}
string deBytetwo(string str)
{
	string seq = "";
	for (int i = 0; i < str.length(); i++)
	{
		unsigned char code = str[i];
		if (code < 128)
			seq += str[i];
		else
		{
			unsigned char len = str[i] - 128 - 1;
			int runs = len;
			int k = 0;
			while (k < runs)
			{
				seq += str[i - 1];
				k++;
			}
		}
	}
	return seq;
}
void preQV(string infile, string outfile)
{
	ifstream fin;
	fin.open(infile);
	if (!fin.is_open()){ cout << "Can't read file.\n"; exit(-1); }
	ofstream fout;
	fout.open(outfile, ios_base::out | ios_base::trunc | ios_base::binary);
	if (!fout.is_open()){ cout << "Can't write file.\n"; exit(-1); }
	string seq;
	while (getline(fin, seq))
	{
		string temp ="";
		if(seq.length()==0){
			fout << endl;
			// continue;
			}
		else
		   {
			temp = Bytetwo(seq);
			fout << temp << endl;}

	}
	fout.close();
	fin.close();
	// __line__
	if(remove(infile.c_str()) == -1){std :: cout << "remove " << infile << " failed.";}
	return;
}
void depreQV(string infile, string outfile)
{
	ifstream fin;
	fin.open(infile, ios_base::in | ios_base::binary);
	if (!fin.is_open()){ cout << "Can't read file.\n"; exit(-1); }
	ofstream fout;
	fout.open(outfile, ios_base::out | ios_base::trunc);
	if (!fout.is_open()){ cout << "Can't write file.\n"; exit(-1); }
	string seq;
	while (getline(fin, seq))
	{
		string temp = deBytetwo(seq);
		if (temp.length() == 1)
			break;
		fout << temp << endl;
	}
	fout.close();
	fin.close();
	if(remove(infile.c_str()) == -1){std :: cout << "remove " << infile << " failed.";}
	return;
}