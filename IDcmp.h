/*
 * @Author: your name
 * @Date: 2021-07-18 16:43:53
 * @LastEditTime: 2021-07-19 15:15:58
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CIndex+-github/IDcmp.h
 */
#ifndef IDCMP_H
#define IDCMP_H

#include<string>
#include<vector>
#include<fstream>
#include<iostream>
#include"EliasCode.h"
#include"loadkit.h"
#include"savekit.h"
#include"BaseClass.h"
#include"IDcmp.h"
#include"FM.h"
#include "InArray.h"
#include <dirent.h>
#include<regex>

bool is_ch(char c);
int initial_regular(std :: string mode, std :: vector<std :: string> &fields,std :: vector<char> &regular);
bool is_numeric(std :: string str);
void split(std::vector<std::string> input, std::vector<std::string> &fields, std::vector<char> &regular,
	std::string IdenticalFile, std::string IncreFile,
	std::string RandomFile, std::string RunLengthFile, std::string AlphaNumFile,int blockSize,int sample, int shape);
void cmpID(std :: vector<std :: string>ID,int blockSize, int sample,int shape, std :: string savePath);
std :: string retainIdenFields(std :: string input, std :: string &firstIdSeq,int &fields_num);
bool getIDfileds(std ::string prefixinput, vector<FM*> &v_FMid, vector<InArray*> &v_IAid, vector<string> &v_strid, vector<char> &v_strsplitid, vector<int> &v_pos, vector<int> &v_length,char *&first_id_seq);
#endif