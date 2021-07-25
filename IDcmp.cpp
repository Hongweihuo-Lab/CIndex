
#include"IDcmp.h"
#include"create_newLevel.h"
#include"DNAcmp.h"
using namespace std;
bool is_ch(char c)
{
	if(c == '@')
		return true;
	else if(c >= '0' && c <= '9')
		return true;
	else if(c >= 'a' && c <= 'z')
		return true;
	else if(c >= 'A' && c <= 'Z')
		return true;
	return false;
}
int initial_regular(std :: string mode, std :: vector<std :: string> &fields,std :: vector<char> &regular)//regular jiangefuhao
{
	int len = mode.size();
	std :: string temp = "";
	for(int i = 0; i < len; i++)
	{
		if(!is_ch(mode[i]))
		{
			regular.push_back(mode[i]);
			if(temp != "")
			{
				fields.push_back(temp);
				temp = "";
			}
		}
		else
		{
			temp += mode[i];
		}

	}
	if(temp != "")
		fields.push_back(temp);
	int fields_num = fields.size();
	return fields_num;
}

bool is_numeric(std :: string str)
{
	for(int i = 0; i < str.size(); i++)
	{
		if(str[i] > '9' || str[i] < '0')
			return false;
	}
	return true;
}

void split(std ::vector<std ::string> input, std ::vector<std ::string> &fields, std ::vector<char> &regular,
		   std ::string IdenticalFile, std ::string IncreFile, std ::string RandomFile,
		   std ::string RunLengthFile, std ::string AlphaNumFile, int blockSize, int sample,int shape)
{
	int part = initial_regular(input[0],fields,regular); //fields
	cout << "part = " << part << endl;
	std :: vector<std :: vector<std :: string> > identical(part);
	std :: vector<std :: vector<std :: string> > increment;//1_
	std :: vector<std :: vector<std :: string> > random(part);//2_
	std :: vector<std :: vector<std :: string> > RunLen(part);//3_
	std :: vector<std::vector<std::string> > alphaNum(part);//4_

	int *table = new int [part];
	for(int i = 0; i < part; i++)
		table[i] = 0;
	int partnum = 0;
	std :: string temp = "";
	std :: vector<std :: string> pre; //
	for(int i = 0; i < fields.size(); i++)
		pre.push_back(fields[i]);
	int regular_size = regular.size();
	for(int i = 1; i < input.size(); i++)
	{
		partnum = 0;
		int regular_num=0;
		for(int j = 0; j < input[i].size() + 1; j++) 
		{
			if((j == input[i].size()) || ((regular_num < regular_size) && input[i][j] == regular[regular_num]))
			{
				if(temp.length()>0){
					if(temp != fields[partnum])
					{
						if(is_numeric(temp) && table[partnum] != 2)
						{
						if(atoi(temp.c_str()) >= atoi(pre[partnum].c_str()))
						{
							table[partnum] = 1;
							pre[partnum] = temp;
						}
						else
						{
							table[partnum] = 2;
						}

						}
						else
						{
							table[partnum] = 2;
						}
					}
					else if(temp == fields[partnum] && table[partnum] == 0)
					{
						table[partnum] = 0;
					}
					else if(temp == fields[partnum] && table[partnum] == 1)
					{
						table[partnum] = 2;
					}
					else if(temp == fields[partnum] && table[partnum] == 2)
					{
						table[partnum] = 2;
					}
					else
					{
						std :: cout << "table class error: " << input[i][j] << regular[regular_num]<<std :: endl;
						exit(-1);
					}
					partnum++;
				}
				regular_num++;
				temp = "";
			}
			else
			{
				temp += input[i][j];
			}
		}
	}
	partnum = 0;
	temp = "";
	for(int i = 1; i < input.size(); i++)
	{
		partnum = 0;
		for(int j = 0; j < input[i].size() + 1; j++)
		{
			if((j == input[i].size() || !is_ch(input[i][j]))&&  temp.size()>0 )
			{

				if(table[partnum]==0)
				{
					if(identical[partnum].size() == 0)
						identical[partnum].push_back(temp);
					temp = "";
				}
				else
				{
					if(is_numeric(temp))
						random[partnum].push_back(temp);
					else
						alphaNum[partnum].push_back(temp);
					temp = "";
				}
				partnum++;

			}
			else
			{
				temp += input[i][j];
			}
		}
	}
	for(int i = 0; i < part; i++)
	{
		if(table[i] == 3)
		{
			for(int j = 0; j < random[i].size(); j++)
			{
				RunLen[i].push_back(random[i][j]);
			}
			random[i].clear();
		}
		else if (table[i] == 4)
		{
			for (int j = 0; j < random[i].size(); j++)
			{
				alphaNum[i].push_back(random[i][j]);
			}
			random[i].clear();
		}
	}
	cout << "field type:\n";
	for (int i = 0; i < part; i++)
		std::cout << table[i] << " ";
	std::cout << std::endl;
	char lenflag;

	if(table[part-1]==0)//equal length
		lenflag = '0';
	else
		lenflag = '1';

	int partsize = 0;
	int sumsize = 0;
	std :: string filename2;

	IdenticalFile += ".txt";
	ofstream fout1;
	fout1.open(IdenticalFile, std::ios_base::out | std::ios_base::trunc);
	if(!fout1.is_open())
	{
		std::cout << "Can't open file:" << IdenticalFile << std::endl;
		exit(-1);
	}
	fout1 << lenflag << std :: endl;
	fout1 << input[0] << std :: endl;
    fout1 << part << std :: endl;
	fout1.close();
	FM*test;
	for(int i = 0; i < increment.size(); i++)
	{


		if(increment[i].size() > 0)
		{
			std :: string temp = IncreFile;
			temp += num2str(i);
			temp += ".txt";

			fout1.open(temp,ios_base :: out | ios_base :: trunc);
			if(!fout1.is_open())
			{
				std :: cout << "Can't open file:" << temp << std :: endl;
				exit(-1);
			}
			//part_num
			fout1 << i << std :: endl;

			//first value
			fout1 << increment[i][0] << endl;
			//how many diffs
			u64 size = increment[i].size() - 1;
			fout1 << size << endl;

			for(int j = 1; j < increment[i].size(); j++)
			{
				int diff = atoi(increment[i][j].c_str()) - atoi(increment[i][j - 1].c_str());
				fout1 << diff << endl;
			}
			fout1.close();

			test = new FM(temp.c_str(),32,blockSize, shape, 1,0,true);
			// partsize = test -> SizeInByte() / 1024;
			partsize =1;//wch
			sumsize += partsize;
			filename2 = temp;
			filename2 += ".CMP.txt";
			test -> Save(filename2.c_str());
		}
	}
	//Random
	for(int i = 0; i < random.size(); i++)
	{
		if(random[i].size() > 0)
		{
			std::string temp = RandomFile;
			temp += num2str(i);
			u64 total_num = random[i].size();
			int max = -1;
			for (u64 j = 0; j < total_num; j++)
			{
				int value = atoi(random[i][j].c_str());
				max = max < value ? value : max;
			}
			int singleBitlen = log2(max) + 1;
			temp += "_" + num2str(singleBitlen);
			temp += ".txt";
			InArray in_random(total_num, singleBitlen);
			for (u64 j = 0; j < total_num; j++)
			{
				in_random.SetValue(j, atoi(random[i][j].c_str()));
			}
			savekit s(temp.c_str());
			in_random.write(s);
			s.close();
		}
	}
	//AlphaNum
	for (int i = 0; i < alphaNum.size(); i++)
	{
		if (alphaNum[i].size() > 0)
		{
			std::string temp = AlphaNumFile;
			int length = alphaNum[i][0].length();
			temp += num2str(i);
			temp += "_" + num2str(length);
			temp += ".txt";
			fout1.open(temp, ios_base::out | ios_base::trunc);
			if (!fout1.is_open())
			{
				std::cout << "Can't open file:" << temp << std::endl;
				exit(-1);
			}
			u64 total_num = alphaNum[i].size();
			for (u64 j = 0; j < total_num; j++)
			{
				fout1 << alphaNum[i][j];
			}
			fout1.close();

			test = new FM(temp.c_str(), blockSize,sample,shape, 1, 0,true);
			partsize = test->SizeInByte() / 1024;
			sumsize += partsize;
			filename2 = temp;
			filename2 += ".CMP.txt";
			string filename3 = filename2 + ".SAL";
			test->Save(filename2.c_str());
			test->SaveSAL(filename3.c_str(),0);
		}
	}
	for(int i = 0; i < RunLen.size(); i++)
	{
		if(RunLen[i].size() > 0)
		{
			std :: string temp = RunLengthFile;
			temp += num2str(i);
			temp += ".txt";
			fout1.open(temp,ios_base :: out| ios_base :: trunc);
			if(!fout1.is_open())
			{
				std :: cout << "Can't open file:" << temp << std :: endl;
				exit(-1);
			}

			fout1 << i << std :: endl;

			std :: string pre = RunLen[i][0];
			u64 runsnum = 1;
			for(int j = 1; j <  RunLen[i].size(); j++)
			{
				if(RunLen[i][j] == pre)
				{runsnum++;continue;}
				else
				{

					//pre
					fout1 << pre << ":";
					//runsnum
					fout1 << runsnum << endl;

					pre = RunLen[i][j];
					runsnum = 1;
					continue;
				}
			}
			//pre
			fout1 << pre << std :: endl;
			//runsnum
			fout1 << runsnum << endl;
			runsnum = 1;

			std :: string end = "\0";

			fout1 << end << std :: endl;
			fout1 << 1 << std :: endl;
			fout1.close();
			test = new FM(temp.c_str(), blockSize,sample,shape,1,0,true);
			partsize = test -> SizeInByte() / 1024;
			sumsize += partsize;
			filename2 = temp;
			filename2 += ".CMP.txt";
			string filename3 = filename2 + ".SAL";
			test -> Save(filename2.c_str());
			test -> SaveSAL(filename3.c_str(),0);
		}
	}
	delete [] table;
	std :: cout << "ID compress complete. \n";
	return;
}

void cmpID(std :: vector<std :: string>ID,int blockSize, int sample,int shape, std :: string savePath)
{
	std :: cout << "------------ID Compression------------\n";
	std :: vector<std :: string> fields;
	std :: vector<char> regular;
	split(ID, fields, regular, savePath +"0_0", savePath +"1_",
		savePath +"2_", savePath +"3_", savePath +"4_",blockSize,sample,shape);
	std :: cout << "-------ID compression end------\n";
	return;
}

bool getIDfileds(std ::string prefixinput, vector<FM *> &v_FMid, vector<InArray *> &v_IAid, vector<string> &v_strid, vector<char> &v_strsplitid, vector<int> &v_pos, vector<int> &v_length,char *&first_id_seq)
{
	std ::cout << "retainIdFields....\n";
	//deIDseq();
	int table_num = 0;
	std ::string firstIdSeq;
	int fields_num = 0;
	std::string lenflag = retainIdenFields(prefixinput + "0_0.txt", firstIdSeq, fields_num);
	int templen = firstIdSeq.length();
	first_id_seq = new char[templen+1];
	for(int y = 0;y < templen;++y)
		first_id_seq[y] = firstIdSeq[y];
	first_id_seq[templen] = '\0';
	int part = initial_regular(firstIdSeq, v_strid, v_strsplitid);
	if (part != fields_num)
	{
		std ::cout << "part != fields_num.\n";
		exit(-1);
	}
	v_FMid.resize(part, 0);
	v_IAid.resize(part,0);
	vector<int> va(part);
	vector<int> vpos(part, 0);
	///
	string dirname = prefixinput;
	DIR *dp;
	struct dirent *dirp;
	regex reg_obj_IA("2_(\\d+)_(\\d+).txt", regex::icase);
	smatch matchResult_IA;
	smatch matchResult_FM;
	regex reg_obj_FM("4_(\\d+)_(\\d+).txt.CMP.txt", regex::icase);
	if ((dp = opendir(dirname.c_str())) == NULL)
		cout << "Can't open " << dirname << endl;
	while ((dirp = readdir(dp)) != NULL){
		string strfilename(dirp->d_name);
		if (dirp->d_type == 8 && regex_match(strfilename, matchResult_IA, reg_obj_IA))
		{
			if (matchResult_IA.size()!=3){
				cout << strfilename << "name mismatch" << endl;
				exit(-1);
			}
			std::ssub_match index = matchResult_IA[1];
			std::ssub_match base_sub_match2 = matchResult_IA[2];
			// cout << index.str() << ":" << base_sub_match2.str() << endl;
			InArray *in = new InArray();
			string strfilename2 = prefixinput + strfilename;
			loadkit s((strfilename2).c_str());
			in->load(s);
			s.close();
			v_IAid[atoi(index.str().c_str())] = in;
			vpos[atoi(index.str().c_str())] = 1;
		}
		else if (dirp->d_type == 8 && regex_match(strfilename, matchResult_FM, reg_obj_FM))
		{
			// string strfilename(dirp->d_name);
			if(matchResult_FM.size() != 3)
			{
				std ::cout << strfilename << "name mismatch" << endl;
				exit(-1);
			}
			std::ssub_match index = matchResult_FM[1];
			std::ssub_match length = matchResult_FM[2];
			// cout << base_sub_match1.str() << " " << base_sub_match2.str() << endl;
			FM *fm = new FM();
			string strfilename2 = prefixinput + strfilename;
			fm->Load(strfilename2.c_str());
			v_FMid[atoi(index.str().c_str())] = fm;
			cout << atoi(index.str().c_str()) << ";" << atoi(length.str().c_str())<< endl;
			va[atoi(index.str().c_str())] = atoi(length.str().c_str());
			vpos[atoi(index.str().c_str())] = 2;
		}
	}
	v_length = va;
	v_pos = vpos;
	closedir(dp);
	return false;
}

std :: string retainIdenFields(std :: string input, std :: string &firstIdSeq,int &fields_num)
{
	ifstream fin;
	fin.open(input,std :: ios_base :: in);
	if(!fin.is_open())
	{
		std :: cout << "Can't open file :" << input << std :: endl;
		exit(-1);
	}
	std :: string lenflag;
	getline(fin,lenflag);
	//input[0]
	getline(fin,firstIdSeq);

	std :: string temp;
	getline(fin,temp);
	fields_num = atoi(temp.c_str());
	fin.close();
	return lenflag;
}