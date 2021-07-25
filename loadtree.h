#ifndef LOADTREE_H
#define LOADTREE_H
#include"BaseClass.h"
#include"loadkit.h"
#include<map>
huffmanTree LoadWTTree(loadkit &s,u32 alphabetsize);
int Loadlabel(loadkit & s,huffmanTree node);

#endif