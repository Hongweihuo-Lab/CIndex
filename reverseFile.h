#ifndef REVERSEFILE_H
#define REVERSEFILE_H
#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include"create_newLevel.h"
#include"BaseClass.h"
bool isnum(std :: string str);
int getseqnum(std :: string &seq);
void reverse_file(int start_ch, int end_ch,const std :: string inPrefix,std :: string outputfile,int qvnum);
void testfunc(std :: string rever_file, std :: string orig_file);
void combineto1(std :: string fileid,std :: string filedna,std :: string fileqv,std :: string output);
int getlength(std :: string tempid,std :: ofstream &fout);
#endif