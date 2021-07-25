#ifndef CREATE_NEWLEVEL
#define CREATE_NEWLEVEL
#include<vector>

//void create_newlevel(std :: vector<std :: string> &QV, double gmd,int level, int originalSize,char most);
void create_newlevel(std :: vector<int> key,std :: vector<std :: string> &QV,const std :: string filename,int &level,int qvnum,int part, bool part_flag);
std :: string num2str(int level);
unsigned char* runlen(std :: string qvseq,char most);
std :: string de_runlen(std :: string ans,char most);
std :: string rl_2bytes(std :: string seq);
std :: string rl_triple(std :: string seq,char most);
std :: string choose(std :: string qvseq,char most);
char getMostFreqCh(std :: string str);
char get2rdMostCh(std :: string str,char most);
void set1(unsigned char* mask,int i);
unsigned char* trim(std :: string &str, char most);
int countTable(std :: string str);
#endif