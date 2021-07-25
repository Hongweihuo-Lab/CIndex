#ifndef	ELIASCODE_H
#define ELIASCODE_H
#include<iostream>
#include<fstream>
#include<string> 
#include"BaseClass.h"
int getBitsNum(unsigned x);
void Append_g(u32 runs, uchar **buffPPtr, uchar &offset);
int compInc(std :: string file);
void elisGammaDecode(int &num,unsigned char **buffPPtr,unsigned char* offset);


#endif