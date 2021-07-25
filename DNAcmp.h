#ifndef DNACMP_H
#define DNACMP_H
#include"divsufsort64.h"
#include"EliasCode.h"
#include"create_newLevel.h"
#include"statics.h"
#include"create_ref.h"
#include"BaseClass.h"
//#include"huffman.h"
#include<string>
#define BIG 127
unsigned char * FileToT(const char *filename,std :: string &dic,unsigned long long &n,int C[]);
u64 BWT(unsigned char *T,int * SA,unsigned char * bwt,u64 len);
int MTF(unsigned char* bwt, std :: string dic,int reallen);
void deMTF(unsigned char* bwt, int *result, std :: string dic, int reallen);
void decmpDNA(std :: string filename, std :: string outfile);
int inv_bw_transform(unsigned char *bwt, unsigned char *T,unsigned long long n, unsigned long long idx,std :: string dic,int*C);
int writeToFile(const char* filename,unsigned char*T,u64 n);
int DNAgamma(int *result,unsigned long long int len, std :: string filename,std :: string dic,u64 bwtIndex, int *C);
int* deDNAgamma(unsigned long long int &len,std :: string inputfile,std :: string &dic,int *C,u64 &bwtIndex);
int find_index(char ch, std :: string dic);
int cmpDNA(std :: string infilename,std :: string outfilename);
void cmpDNARK(std :: string infilename, std :: string outprefix);
int getkey(std :: string str);
void cmpDNA_2(std :: string origdna, std :: string filename, int file_size_kb,int qvnum,int refnum);
void decmpDNA_2(std :: string origdna, std :: string filename, std :: string outputfile,int file_size_kb,int qvnum,int refnum);
#endif