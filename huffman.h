#ifndef  _HUFFMAN_H
#define _HUFFMAN_H
#include"loadtree.h"
#include"saveTree.h"
#include"BaseClass.h"
int getBlockDataInfo(unsigned char *inbuff, unsigned int len, bool *charMap,unsigned int *charFreq, unsigned int* setSiz);
huffmanTree createHuffman(unsigned char *L, unsigned int len, dataInfo *dInfoPtr);
void generateCodeTable(
	huffmanTree root,
	char codeTable[CHAR_SET_SIZE][CODE_MAX_LEN],
	char code[CODE_MAX_LEN], 
	char nxetCode, 
	int level);
void destroyHuffmanTree(huffmanTree root);
void printCodeTable(dataInfo dinfo);
void AppendHuff(unsigned char *data, unsigned char x, unsigned long long &curbit, dataInfo info);
u32 HuffmanCode(unsigned char* input,unsigned long long n,std :: string outputfile,std :: string dic,u64 bwtIndex,int *C);
int getbit(u8*data, u64 decodebit);
int genResultByTree(int *result,u64 n,u8 *data,huffmanTree root);
int* HuffmanDecode(std :: string inputfile,std :: string &dic,int *C,u64 &bwtIndex,unsigned long long &n);
void printData(u8* data,int k);
#endif