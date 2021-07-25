/*
 * @Author: your name
 * @Date: 2021-07-18 13:50:15
 * @LastEditTime: 2021-07-18 16:57:16
 * @LastEditors: your name
 * @Description: In User Settings Edit
 * @FilePath: /CIndex+-github/CreateText.cpp
 */
#include"CreateText.h"
extern int seqlength;
int create_text(std :: string inputfile,std :: vector<std :: string> &ID,std :: string dnastream,std :: string qvstream,int &dna__len)
{
	std :: string seq;
	std :: vector<std :: string> read; 
	std :: vector<std :: string> QV;   


	std :: ifstream fin;
	std :: ofstream fout;
	std :: ofstream fout2;
	std :: ofstream fout3;
	fout.open(dnastream,std :: ios_base :: out | std :: ios_base :: trunc);
	if(!fout.is_open())
	{
		std :: cout << "Can't open the file\n";
		exit(EXIT_FAILURE);
	}

	fout2.open(qvstream,std :: ios_base :: out | std :: ios_base :: trunc);
	if(!fout2.is_open())
	{
		std :: cout << "Can't open the file\n";
		exit(EXIT_FAILURE);
	}

	fin.open(inputfile,std :: ios_base :: in);
	if(!fin.is_open())
	{
		std :: cout << "Can't open the file\n";
		exit(EXIT_FAILURE);
	}

	int row = 0;
	int seqlength1 = 0;
	std::string::size_type idx;
	bool bSamelength = true;
	getline(fin,seq);
	ID.push_back(seq);
	row++;
	bool fixlength = true;
	int fistlength = 0;
	int cnt = 0 ;
	while(getline(fin,seq))
	{
		if(row % 4 == 0)
		{
			ID.push_back(seq);
		}
		else if(row % 4 == 1)
		{

			fout << seq << std :: endl;
			if(cnt==0){
				fistlength = seq.length();
				cnt++;
			}
			else if (fixlength ==true)
			{
				if (fistlength != seq.length()){
					fixlength = false;
				    seqlength1=1;}
			}
		}
		else if(row % 4 == 3)
		{
			fout2 << seq << std :: endl;
		}
		row++;
		if(row == 4)row = 0;
	}
	fin.close();
	if(seqlength1==0)
		dna__len=fistlength;
	return seqlength1;
}