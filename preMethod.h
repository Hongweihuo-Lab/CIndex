#ifndef PREMETHOD_H
#define PREMETHOD_H
#include<iostream>
#include<string>
#include<fstream>
#include<vector>
#include<map>
#include<stdio.h>
#include"BaseClass.h"
#include<iomanip>
using namespace std;
void initialize(int *charMap, map<string, int> &dict, map<string, int>&countT);
void initialize2(int *charMap, unsigned char*table, map<int, string> &dict2);

void PackDNA(int seqlength, string inputfile, string outputfile);
void tripleBase(string str, string &ans);
void run_length(string temp, string &ans);
void unpackDNA(string inputfile, string outputfile);

string trimDNA(string temp);
unsigned char* Getfile(const char *filename, unsigned long long&linelength);


void showcountT();
string Bytetwo(string str);
void preQV(string infile, string outfile);
string deBytetwo(string str);
void depreQV(string infile, string outfile);
void testfunc(std::string rever_file, std::string orig_file);
#endif