#include"reverseFile.h"
#include"create_newLevel.h"
extern int seqlength;
bool isnum(std :: string str)
{
	for(int i = 0; i < str.size(); i++)
		if(str[i] > '9' || str[i] < '0')
			return false;
	return true;
}
int getseqnum(std :: string &seq)
{
	std :: string temp = "";
	std :: string ans = "";
	bool flag = false;
	for(int i = 0; i < seq.size(); i++)
	{
		if(seq[i] == ' ')
		{flag = true;continue;}
		else if(!flag && seq[i] >= '0' && seq[i] <= '9')
			temp += seq[i];
		else
			ans += seq[i];
	}
	if(isnum(temp))
	{
		seq = ans;
		return atoi(temp.c_str());
	}
	else
		return -1;
}

void reverse_file(int start_ch, int end_ch,const std :: string inPrefix,std :: string outputfile,int qvnum)
{
	std :: ofstream fout;
	std :: ifstream fin;
	std :: cout << "Rebuild QV file\n";
	std :: string space = "";
	//std :: vector<std :: string> v1(qvnum,space);
	int partNumb = 20;
	std :: vector<std :: vector<std :: string>> seq_part(partNumb);  //save one part of the original file,�������10 parts

	for(int i = 0; i < partNumb; i++)
	{
		for(int j = 0; j < qvnum; j++)
			seq_part[i].push_back(space);
	}



	for(int i = start_ch; i <= end_ch; i++)
	{
		std :: string filename = inPrefix;
		std :: string num = num2str(i);
		filename += num;
		filename += ".txt";

		FILE *fp=fopen(filename.c_str(),"r");
        if(!fp) continue;
		fclose(fp);

		fin.open(filename,std :: ios_base :: in);
		if(!fin.is_open())
		{
			std :: cout << "Can't open the file\n";
			exit(EXIT_FAILURE);
		}
		std :: cout << "Read file :" << filename << std :: endl;
		std ::  string str;
		int partnum = -1;
			//std :: cout << filename << std :: endl;
			//std :: cout << getline(fin,str) << std :: endl;

		while (getline(fin,str))
		{

			if(isnum(str)) // new part
			{
				 partnum = atoi(str.c_str());
			}
			else
			{
				int seqnum  = getseqnum(str);
				if(seq_part[partnum][seqnum] == "")
				{
					seq_part[partnum][seqnum] = str;
					//std :: cout << seq_part[partnum][seqnum] << std :: endl;
				}

				else
					std :: cout << "1 seq_part["<<partnum<<"]["<<seqnum<<"]�ѱ�ռ��\n";
			}
		}
		fin.close();
	}



	for(int i = start_ch; i <= end_ch; i++)
	{
		for(int j = start_ch; j <= end_ch; j++)
		{

			std :: string filename = inPrefix;
			std :: string num = num2str(i);
			filename += num;
			num = num2str(j);
			filename += "_";
			filename += num;
			filename += ".txt";

			FILE *fp=fopen(filename.c_str(),"r");
            if(!fp) continue;
		    fclose(fp);

			fin.open(filename,std :: ios_base :: in);
			if(!fin.is_open())
			{
				std :: cout << "Can't open the file\n";
				exit(EXIT_FAILURE);
			}
			std ::  string str;
			int partnum = -1;
			std :: cout << "Read file :" << filename << std :: endl;
			std :: string partnum_str = "";
			getline(fin,partnum_str);
			while (getline(fin,str))
			{
				int seqnum  = getseqnum(str);
				if(seq_part[i][seqnum] == "")
				{
					seq_part[i][seqnum] = str;
				}
				else
					std :: cout << "2 seq_part["<<i<<"]["<<seqnum<<"]�ѱ�ռ��\n";
			}
			fin.close();
		}

	}

	std :: cout << "Here3 \n";

	fout.open(outputfile,std :: ios_base :: out | std :: ios_base :: trunc);
	if(!fout.is_open())
	{
		 std :: cout << "Can't open the " << outputfile << "\n";
		exit(EXIT_FAILURE);
	}
	int k = 0;
	for(int i = 0; i < partNumb; i++)
	{
		if(seq_part[i][0] != space)
		{

			for(int j = 0; j < seq_part[i].size(); j++)
			{
				if(seq_part[i][j] == space)
				{
					break;
					//std :: cout << "part = " << i << std :: endl;
					//std :: cout << "NO. = " << j << std :: endl;
				}
				fout << seq_part[i][j] << std :: endl;
			}

		}
		else
		{
			std :: cout << "partnum = " << i << std :: endl;
			break;
		}
	}
	std :: cout << "Rubulid success\n";
	return;
}
void testfunc(std :: string rever_file, std :: string orig_file)
{
	std :: ifstream fin1;
	std :: ifstream fin2;
	fin1.open(rever_file,std :: ios_base :: in);
	if(!fin1.is_open()){std :: cout << "can't open " << rever_file ; return;}
	fin2.open(orig_file,std :: ios_base :: in);
	if(!fin2.is_open()){std :: cout << "can't open " << orig_file;return;}

	std :: string str1;
	std :: string str2;
	int i = 0;
	while(getline(fin1,str1) && getline(fin2,str2))
	{
		i++;
		if(str1 != str2)
		{
			if(i % 4 == 3)
				continue;
			else
			{
			std :: cout<< i << "\n" << str1 << "\n" << str2 << std :: endl;
			exit(-1);
		             }
		}

		//std :: cout << i << std :: endl;
	}
	std :: cout << "OK!!\n";
	return;
}
int getlength(std :: string tempid,std :: ofstream &fout)
{
	std :: string templen="";
	bool flag = 0;
	for(int i = 0; i < tempid.size(); i++)
	{
		fout << tempid[i];
		if(tempid[i]=='='){flag=1;continue;}
		if(flag)templen+=tempid[i];
	}
	fout << std :: endl;
	int len = atoi(templen.c_str());
	if (templen == "")return 0;
	return len;
}
void combineto1(std :: string fileid, std :: string filedna, std :: string fileqv, std :: string output)
{
	std :: ifstream finid;
	std :: ifstream findna;
	std :: ifstream finqv;
	std :: cout << "Combine 3 streams\n";
	finid.open(fileid,std :: ios_base :: in);
	findna.open(filedna,std :: ios_base :: in);
	finqv.open(fileqv,std :: ios_base :: in);

	if(!finid.is_open() || !findna.is_open() || !finqv.is_open())
	{
		std :: cout << "File open error\n";
		exit(-1);
	}
	std :: ofstream fout;
	fout.open(output,std :: ios_base :: out | std :: ios_base :: trunc);
	if(!fout.is_open())
	{
		std :: cout << "Can't open file\n";
		exit(-1);
	}

	std :: string tempid,tempdna,tempqv;
	if(seqlength!=0)//ｆｉｘ　ｌｅｎ　ｄｎａ
	while (getline(finid, tempid) && getline(findna, tempdna) && getline(finqv, tempqv))
	{
		fout << tempid << std :: endl;
		fout << tempdna << std :: endl;
		std :: string id3 = "+";
		/*
		for (int i = 1; i < tempid.length(); i++)
		{
			id3 += tempid[i];
		}
		*/
		fout << id3 << std :: endl;

		fout << tempqv << std :: endl;
	}
	else
		while(getline(finid, tempid)  ){
			tempqv = "";
			fout<< tempid << std ::endl;
			int dnalen = atoi(tempid.substr(tempid.find("=")+1).c_str());
			char* pcharDna = new char[1024];
			findna.read(pcharDna,dnalen);
			pcharDna[dnalen] = 0;
			tempdna = pcharDna;
			fout<<tempdna<<std::endl;
			// fout<< tempqv << std::endl;
			if(dnalen>0)
				getline(finqv, tempqv);
			std ::string id3 = "+";
			fout << id3 << std :: endl;
			fout << tempqv << std :: endl;
	}
	finid.close();
	findna.close();
	finqv.close();
	fout.close();
	std :: cout << "Combine 3 streams success\n";
	if(remove(fileid.c_str()) == -1){std :: cout << "remove " << fileid << "failed.";}
	if(remove(filedna.c_str()) == -1){std :: cout << "remove " << filedna << "failed.";}
	if(remove(fileqv.c_str()) == -1){std :: cout << "remove " << fileqv<< "failed.";}
	return;
}