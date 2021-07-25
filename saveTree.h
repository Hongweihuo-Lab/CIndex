#ifndef SAVETREE_H
#define SAVETREE_H
#include"BaseClass.h"
#include"Huffman_WT.h"
#include"savekit.h"
int Savelabel(savekit &s, huffmanTree node);
int SaveWTTree(savekit &s,huffmanTree root);
int SaveNodeData(huffmanTree r,savekit &s);
int SaveNodePosition(huffmanTree r,u32 position,savekit &s);
#endif