#include"DNAcmp.h"
#include"divsufsort.h"
#include"savekit.h"
#include"loadkit.h"
#include<iostream>
#include<fstream>
#include <string.h>
#include"reverseFile.h"

unsigned char * FileToT(const char *filename,std :: string &dic,unsigned long long &n,int C[])
{

	FILE*fp = fopen(filename, "rb"); //get the pointer of file;
	if (fp == NULL) {
		std :: cout << "can not find the file!" << std :: endl;
		exit(0);
	}
	fseek(fp, 0, SEEK_END);//put the pointer on the file end ; 0 represents the pointer overloop 0 Byte from SEEK_END;
    n = ftell(fp) + 1;//return the overloop;
	fseek(fp, 0, SEEK_SET);
	uchar *T = new uchar[n];
	int e = 0;
	int num = 0;
	e = fread(T, sizeof(uchar), n, fp);
	T[n-1] = '\0';

	if (e != n - 1)
	{
		std :: cout << "read the file failed!" << std :: endl;
	}
	else
	{
		std :: cout << "read the file success!" << std :: endl;
	}
	fclose(fp);

	int *charFreq = new int [256];
	bool *charMap = new bool [256];
	memset(charFreq,0,256*sizeof(int));
	memset(charMap,0,256*sizeof(bool));

	for(int i=0;i<n;i++)
		charFreq[T[i]]++;
	int alphabetsize = 0;
	for(int i=0;i<256;i++)
	{	if(charFreq[i])
		{
			alphabetsize++;
			charMap[i]=true;
		}
	}
	//memset(C,0,(alphabetsize+1)*4);
	memset(C,0,256*4);
	C[alphabetsize] = n;
	C[0] = 0;
	int k=1;
	int pre =0;
	for(int i = 0;i < 256; i++)
	{
		if(charFreq[i]!=0)
		{

			C[k]=pre + charFreq[i];
			pre = C[k];
			k++;
		}
	}
	for(int i = 0; i < 256; i++)
	{
		if(charMap[i])
		{
			char c = i;
			dic += c;
		}
	}
	delete [] charFreq;
	delete [] charMap;

	std :: cout << "�ֵ䣺" << std :: endl;
	for(int i = 0; i < dic.size(); i++)
		std :: cout << i << ":" << (int)dic[i] << std :: endl;
	return T;
}

u64 BWT(unsigned char *T,int * SA,unsigned char * bwt,u64 len)
{

	int i=0;
	int index=0;
	u64 bwtIndex = 0;
	for(i=0;i<len;i++)
	{
		index = (SA[i]-1+len)%len;
		bwt[i]=T[index];
		if(index == len - 1)
			bwtIndex = i;
	}
	return bwtIndex;
}

int find_index(char ch, std :: string dic)
{
	for(int i = 0; i < dic.size(); i++)
	{
		if(ch == dic[i])
			return i;
	}
	std :: cout << ch << "�����ֵ�:";
	return -1;
}
int MTF(unsigned char* bwt, int *result, std :: string dic,int reallen)
{
	 //int size = strlen((char *)bwt);
	 for(int i = 0; i < reallen; i++)
	 {
		 int index = find_index(bwt[i],dic);
		 result[i] = index;
		 char c = dic[index];
		 dic.erase(index,1);
		 dic.insert(0,1,c);
	 }
	 return 0;
}

void deMTF(unsigned char* bwt, int *result, std :: string dic, int reallen)
{
	for(unsigned long long i = 0; i < reallen; i++)
	{
		if(result[i] >= dic.size())
			std :: cout << i << ":" << result[i] << std :: endl;
		int index = result[i];
		char ch = dic[index];
		dic.erase(index,1);
		dic.insert(0,1,ch);
		bwt[i] = ch;
	}
	return;
}

int* deDNAgamma(unsigned long long int &len,std :: string inputfile,std :: string &dic,int *C,u64 &bwtIndex)//����result�Ĵ�С
{
	loadkit *loadfile = new loadkit(inputfile.c_str());
	//dic
	int diclen = 0;
	loadfile ->loadi32(diclen);
	uchar* dic_array = new uchar [diclen];
	loadfile ->loadu8array(dic_array,diclen);
	for(int i = 0; i < diclen; i++)
		dic.push_back(dic_array[i]);
	//len
	loadfile ->loadu64(len);
	//C
	loadfile ->loadi32array(C,256);

	//numbytes
	u64 numbytes = 0;
	loadfile ->loadu64(numbytes);
	//sBuff
	unsigned char *sBuff=new unsigned char[numbytes];
	loadfile ->loadu8array(sBuff,numbytes);
	//bwtIndex
	loadfile ->loadu64(bwtIndex);
	int *result = new int [len];
	unsigned char *mBuff=sBuff;
	unsigned char offset = 0;
	for(unsigned long long i = 0; i < len; i++)
	{
		elisGammaDecode(result[i],&mBuff,&offset);
		result[i]--;
	}
	delete [] sBuff;sBuff = NULL;
	delete [] dic_array;dic_array = NULL;
	delete loadfile;loadfile = NULL;
	return result;
}

int DNAgamma(int *result,unsigned long long int len, std :: string filename,std :: string dic,u64 bwtIndex,int *C)
{
	int outsize = len * 8 + 1;
	unsigned char *sBuff=new unsigned char[outsize];
	unsigned char outIndex=0;
	memset(sBuff,0,outsize);
	unsigned char *mBuff=sBuff;


    for(unsigned long long int i = 0; i < len; i++)
	{
		int ind = result[i] + 1; //!!!
		Append_g(ind,&mBuff, outIndex);
	}
	unsigned int sBitLen=(mBuff-sBuff)*8+outIndex;
	u64 numbytes;
	if(sBitLen % 8 == 0)
		numbytes =  (sBitLen / 8);
	else
		numbytes =  (sBitLen / 8) + 1;

	savekit *savefile = new savekit(filename.c_str());
	//dic
	int diclen = dic.size();
	unsigned char * dic_array = new uchar [diclen];
	for(int i = 0; i < diclen; i++)
		dic_array[i] = dic[i];

	savefile ->writei32(diclen);
	savefile ->writeu8array(dic_array,diclen);
	delete [] dic_array;dic_array = NULL;
	//length of T
	savefile ->writeu64(len);

	//C
	savefile ->writei32array(C,256);

	//numbytes
	savefile ->writeu64(numbytes);

	//sBuff
	savefile ->writeu8array(sBuff,numbytes);

	//bwtIndex
	savefile ->writeu64(bwtIndex);

	delete [] sBuff;sBuff = NULL;
	delete savefile;savefile = NULL;


	int sizekb = numbytes / 1024 + 1;
	std :: cout << std :: endl << filename << "GAMMAѹ����" << sizekb << "KB\n";
	return sizekb;
}
int BWT64(unsigned char *T, saidx64_t *SA, unsigned char *bwt, saidx64_t len)
{
	saidx64_t i = 0;
	saidx64_t index = 0;
	u64 bwtIndex = 0;
	for (i = 0; i < len; i++)
	{
		index = (SA[i] - 1 + len) % len;
		bwt[i] = T[index];
		if (index == len - 1)
			bwtIndex = i;
	}
	return bwtIndex;
}
int cmpDNA(std :: string infilename,std :: string outfilename)
{
	unsigned long long n; // length of T
	std :: string dic = "";

	int *C = new int [256];
	uchar* T = FileToT(infilename.c_str(),dic,n,C);

	std :: cout << std :: endl;
	saidx64_t *sa = new saidx64_t[n];
	unsigned char *bwt = new unsigned char [n];
	divsufsort64((unsigned char *)T, (saidx64_t *)sa, n);
	std :: cout << "sa�������--------\n";
	u64 bwtIndex = BWT64(T,sa,bwt,n);

	delete [] sa;sa = NULL;
	delete [] T;T = NULL;

	int *result = new int [n];
	MTF(bwt, result, dic,n);

	std :: ofstream fout;
	fout.open("C:\\Users\\liuxi\\Desktop\\NEW_FASTQ\\outputQV\\result_level0_18.txt",std :: ios_base :: out | std :: ios_base :: trunc);
	for(int i = 0; i < n; i++)
	{
		int ch = result[i];
		fout << ch << " ";
		if((i + 1) % 30 == 0)fout << endl;
	}
	fout.close();
	std :: cout << "MTF���--------\n";
	delete [] bwt;bwt = NULL;
	int sizekb = DNAgamma(result,n, outfilename,dic,bwtIndex,C);
	//int sizekb = HuffmanCode((unsigned char*)result,n,outfilename,dic,0,C);
	delete [] result; result = NULL;
	return sizekb;
}
int getkey(std :: string str)
{
	int len = str.size();
	unsigned long long sum = 0;
	for(int i = 0; i < len; i++)
	{
		sum += (str[i] << (len - 1 - i));
	}
	int hash = sum % BIG;
	return hash;
}
void cmpDNARK(std :: string infilename, std :: string outprefix)
{
	std :: cout << "DNA hash.....\n";
	std :: fstream fin;
	fin.open(infilename,std :: ios_base :: in);
	if(!fin.is_open())
	{std :: cout << "Can't open file " << infilename << std :: endl;return;}
	std :: string seq;
	std :: vector<std :: vector<std :: string>>hash (BIG);
	u64 i = 0;
	while(getline(fin,seq))
		hash[getkey(seq)].push_back(seq);
	std :: ofstream fout;

	for(int j = 0; j < BIG; j++)
	{
		std :: string temp = outprefix;
		if(hash[j].size() > 0)
		{
			temp += num2str(j);
			temp += ".txt";
			fout.open(temp,std :: ios_base :: out | ios_base :: app);
			if(!fout.is_open())
			{std :: cout << "Can't open file " << temp << std :: endl;}
			for(u64 k = 0; k < hash[j].size(); k++)
			{
				fout << hash[j][k] << std :: endl;
			}
			hash[j].clear();
			fout.close();
		}
	}
	std :: cout << "DNA hash complete.\n";
	return;
}
void cmpDNA_2(std :: string origdna, std :: string filename, int file_size_kb,int qvnum,int refnum)
{
#if 0
	std :: cout << "------------����DNA����------------\n";
	//cluster
	std :: ifstream fin;
	std :: vector<std :: string> read;
	char ch;
	fin.open(origdna,std :: ios_base :: in);
	if(!fin.is_open())
	{
		std :: cout << "Can't open the file\n";
		exit(EXIT_FAILURE);
	}

	//int qvnum = 10000;

	std :: vector <std :: string> ref;

	int part = 0; //��¼Ϊ�ڼ����֣�ÿqvnum��Ϊһ����

	int row = 0;
	int level = refnum - 1;
	int consor_num = 0;
	std :: string seq = "";
	int seqNO = 0;
	while(fin.get(ch))
	{

		if(ch == ' ')continue;
		else if(ch != '\n')
			{seq += ch;}
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
				read.push_back(seq2);
				seqNO++;
			}
			seq = ""; //���³�ʼ��Ϊ��
			row++;
		}
		if(read.size() == qvnum)
		{
			std :: cout << "����part" << part << std :: endl;
			std :: cout << "---------------------------------------"<< std :: endl;
			int C[256] = {0};
			if(part == 0)
			{
				cmp_chFreq(read, C);
				createRef(read,C,refnum,ref);
				for(int i = 0; i < ref.size(); i++)
					std :: cout << ref[i] << std :: endl;
			}

			level_1("D:\\BioStudy\\MyFastq\\MyFastq\\outputDNA\\level",read,ref,part);
		    bool part_flag = false;
			while(read.size() > 0)
			{
				create_newlevel(read,"D:\\BioStudy\\MyFastq\\MyFastq\\outputDNA\\level",level,qvnum,part,part_flag);
				part_flag = true;
			}
			part++;
			read.clear();
			seqNO = 0;
		}

	}
	seqNO = 0;
	level_1("D:\\BioStudy\\MyFastq\\MyFastq\\outputDNA\\level",read,ref,part);
	bool part_flag = false;
    while(read.size() > 0)
	{
		create_newlevel(read,"D:\\BioStudy\\MyFastq\\MyFastq\\outputDNA\\level",level,qvnum,part,part_flag);
		part_flag = true;
	}
	if(read.size() > 0)std :: cout << "read.size() > 0" << std :: endl;
	read.clear();
	fin.close();
#endif
	int time = 0;
	int sumsize = 0;
	for(int j = 4; j <= 9; j++)
		{

			std :: string temp = "D:\\BioStudy\\FMindex\\FMindex\\outputData\\level";
			std :: string num = num2str(j);
			temp += num;
			temp += ".txt";

			FILE *fp=fopen(temp.c_str(),"r");
            if(!fp) continue;
		    fclose(fp);
			time++;
			std :: string outfile = (temp + ".CMP.txt");
			sumsize += cmpDNA(temp,outfile);

		}
	std :: cout << time << " ѹ�����С��" << sumsize<< "KB\n";
	std :: cout << "��ѹ���ʣ�"<< (double)sumsize / 9132 << std :: endl;
#if 0
	//bwt compress
	int sumsize = 0;
	for(int i =35; i <= 84; i++)
	{

		for(int j = 35; j <= 84; j++)
		{

			std :: string filename = "D:\\BioStudy\\MyFastq\\MyFastq\\outputDNA\\level";
			std :: string num = num2str(i);
			filename += num;
			num = num2str(j);
			filename += "_";
			filename += num;
			filename += ".txt";

			FILE *fp=fopen(filename.c_str(),"r");
            if(!fp) continue;
		    fclose(fp);

			std :: string outfile = filename + "_cmp.txt";
			sumsize += cmpDNA(file_size_kb,filename,outfile);
		}
		std :: string filename = "D:\\BioStudy\\MyFastq\\MyFastq\\outputDNA\\level";
		std :: string num = num2str(i);
		filename += num;
		filename += ".txt";
		FILE *fp=fopen(filename.c_str(),"r");
        if(!fp) continue;
		fclose(fp);
		std :: string outfile = filename + "_cmp.txt";
		sumsize += cmpDNA(file_size_kb,filename,outfile);
	}

	//cin.get();
	//sumsize += 15;
	std :: cout << "DNA����ѹ�����С��" << sumsize<< "KB\n";
	std :: cout << "DNA��ѹ���ʣ�"<< (double)sumsize / file_size_kb << std :: endl;
#endif
	return;
}
//DNA��ѹ
void decmpDNA_2(std :: string origdna, std :: string prefix, std :: string outputfile,int file_size_kb,int qvnum,int refnum)
{

	for(int i =35; i <= 84; i++)
	{

		for(int j = 35; j <= 84; j++)
		{
			std :: string temp = "";
			temp += prefix;
			//std :: string filename = "D:\\BioStudy\\MyFastq\\MyFastq\\outputDNA\\level";
			std :: string num = num2str(i);
			temp += num;
			num = num2str(j);
			temp += "_";
			temp += num;
			temp += ".txt";

			FILE *fp=fopen(temp.c_str(),"r");
            if(!fp) continue;
		    fclose(fp);

			std :: string outfile = temp + "_cmp.txt";
			decmpDNA(temp,outfile);
		}
		std :: string temp = "";
			temp += prefix;
		std :: string num = num2str(i);
		temp += num;
		temp += ".txt";
		FILE *fp=fopen(temp.c_str(),"r");
        if(!fp) continue;
		fclose(fp);
		std :: string outfile = temp + "_cmp.txt";
		decmpDNA(temp,outfile);
	}
	//reverse
	reverse_file(35, 84,prefix,outputfile,qvnum);
	return;
}
int inv_bw_transform(unsigned char *bwt, unsigned char *T,unsigned long long n, unsigned long long idx,std :: string dic,int*C)
{
	//C map to real character
	uchar Creal[256] = {0};
	int count = 0;
	for(int i = 0; i < dic.size(); i++)
	{
		Creal[dic[i]] = count++;
	}
	cout << "n = " << n << endl;
	int OCCtable[256] = {0};
	u64 * LF = new u64 [n];
	for(u64 i = 0; i < n; i++)
	{
		if(bwt[i] != 0)
			LF[i] = OCCtable[bwt[i]] + C[Creal[bwt[i]]];
		else
			LF[i] = OCCtable[bwt[i]] + 0;
		OCCtable[bwt[i]]++;
	}

	u64 I = idx;
	for(u64 k = n - 1; k >= 0; k--)
	{
		T[k] = bwt[I];
		I = LF[I];
		if(k == 0)break;
	}
	delete [] LF;LF = NULL;
	return 0;
}
int writeToFile(const char* filename,unsigned char*T,u64 n)
{
	FILE*fp = fopen(filename, "wb"); //get the pointer of file;
	if (fp == NULL) {
		cout << "can not find the file!" << endl;
		exit(0);
	}

	u64 e = 0;
	e = fwrite(T, sizeof(uchar), n, fp);
	if (e != n)
	{
		cout << "write the file failed!" << endl;
	}
	else
		cout << "write the file success!" << endl ;
	fclose(fp);
	return 0;
}
void decmpDNA(std :: string filename, std :: string outfile)
{
	std :: cout << "decompress DNA " << filename << std :: endl;
	unsigned long long len = 0;
	std :: string dic;
	int *C = new int [256];
	u64 bwtIndex = 0;
	int * result = deDNAgamma(len,filename,dic,C,bwtIndex);
	//int * result = HuffmanDecode(filename,dic,C,bwtIndex,len);
	unsigned char* bwt = new unsigned char [len];
	deMTF(bwt, result, dic, len);
	delete [] result;
	unsigned char* T = new unsigned char [len];
	inv_bw_transform(bwt, T,len, bwtIndex,dic,C);
	delete [] C;C = NULL;
	delete [] bwt;bwt = NULL;
	writeToFile(outfile.c_str(),T,len-1);
	delete [] T;T = NULL;
	std :: cout << filename << "decompress complete\n";
	return;
}
