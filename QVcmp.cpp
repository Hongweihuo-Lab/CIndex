#include"QVcmp.h"
#include"statics.h"
#include"DNAcmp.h"
#include<fstream>
#include <string.h>

void cmpQV(std :: string origQV,int qvnum,int refnum)
{

	std :: cout << "------------处理QV序列------------\n";
	std :: ifstream fin;
	std :: vector<std :: string> QV; 
	std :: vector<int>key;
	char ch;
	fin.open(origQV,std :: ios_base :: in);
	if(!fin.is_open())
	{
		std :: cout << "Can't open the file\n";
		exit(EXIT_FAILURE);
	}
	std :: vector <std :: string> ref;
	int part = 0; //记录为第几部分，每qvnum条为一部分
	int row = 0;
	int level = refnum - 1;
	int consor_num = 0;
	std :: string seq = "";
	int seqNO = 0;

	int table[128] = {0};
	int num = 0;
	
	while(fin.get(ch))
	{
		if(ch == ' ')
			continue;
		else if(ch != '\n')
		{
			seq += ch;
			if(table[ch] == 0)
			{
				table[ch]=1;
				num++;
			}
		}
		else if(ch == '\n' && seq == "")
				row++;
		else 
		{
			if(seq != "")
			{
				std :: string seq2 = "";
				seq2 += num2str(seqNO);
				seq2 += ' ';
				seq2 += seq;
				QV.push_back(seq2);
				key.push_back(num);
				seqNO++;
				num = 0;
				memset(table,0,128 * sizeof(int));
			}
			seq = ""; //重新初始化为空
			row++;
		}
		if(QV.size() == qvnum)
		{
			
			std :: cout << "处理part" << part << std :: endl;
			std :: cout << "---------------------------------------"<< std :: endl;
			
			bool part_flag = false;
			create_newlevel(key,QV,"C:\\Users\\liuxi\\Desktop\\NEW_FASTQ\\outputQV\\level",level,qvnum,part,part_flag);
			
			/*while(QV.size() > 0)
			{
			    consor_num++;
				create_newlevel(key,QV,"C:\\Users\\liuxi\\Desktop\\MyFastq_block\\outputQVBLK\\level",level,qvnum,part,part_flag);
				part_flag = true;
			}*/
			
			part++;
			
			key.clear();
			seqNO = 0;
			continue;
		}
	}
	seqNO = 0;
	bool part_flag = false;
	std :: cout << "处理part" << part << std :: endl;
	std :: cout << "---------------------------------------"<< std :: endl;
    while(QV.size() > 0)
	{
	    consor_num++;
		create_newlevel(key,QV,"C:\\Users\\liuxi\\Desktop\\NEW_FASTQ\\outputQV\\level",level,qvnum,part,part_flag);
		//part_flag = true;
	}
	if(QV.size() > 0)std :: cout << "QV.size() > 0" << std :: endl;
	key.clear();
	fin.close();
	std :: cout << "-------QV cluster完成------\n";
	return;
}