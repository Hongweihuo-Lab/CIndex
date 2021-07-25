#ifndef CREATE_TEF_H
#define CREATE_TEF_H
#include<iostream>
#include<string>
#include<vector>
#define COS_45 0.7071
#define COS_60 0.5
#define COS_25 0.9063
#define COS_20 0.9396
#define COS_75 0.2588
#define COS_15 0.9659
#define COS_30 0.866
#define COS_80 0.1736
#define COS_50 0.6427
#define	COS_55 0.5736
#define COS_85 0.0871
int getmin(int x, int y);
//int score(std :: string str1, std :: string str2);
double cos(int vec[2][128],int size);
double score(std :: string str1, std :: string str2);
void createRef(std :: vector<std :: string> QV,int *C,int num, std :: vector <std :: string> &ref);
void level_1(std :: string Filename,std :: vector<std :: string> &QV,std :: vector <std :: string> ref,int part);


#endif