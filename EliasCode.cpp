#include <string.h>
#include"EliasCode.h"
int bitsNumTbl[256] = {//plusOne与gamma差值计算加速表
	-1, 0, 1, 1, 2, 2, 2, 2,3, 3, 3, 3, 3, 3, 3, 3,
	4, 4, 4, 4, 4, 4, 4, 4,	4, 4, 4, 4, 4, 4, 4, 4,
	5, 5, 5, 5, 5, 5, 5, 5,	5, 5, 5, 5, 5, 5, 5, 5,
	5, 5, 5, 5, 5, 5, 5, 5,	5, 5, 5, 5, 5, 5, 5, 5,
	6, 6, 6, 6, 6, 6, 6, 6,	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,	6, 6, 6, 6, 6, 6, 6, 6,
	6, 6, 6, 6, 6, 6, 6, 6,	6, 6, 6, 6, 6, 6, 6, 6,
	7, 7, 7, 7, 7, 7, 7, 7,	7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7,	7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7,	7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7,	7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7,	7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7,	7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7,	7, 7, 7, 7, 7, 7, 7, 7,
	7, 7, 7, 7, 7, 7, 7, 7,	7, 7, 7, 7, 7, 7, 7, 7};
int getBitsNum(unsigned x)
{
	if (x<256){
		return bitsNumTbl[x];
	}
	int n = 1;
	if (x == 0) return -1;
	if ((x >> 16) == 0) { n = n + 16; x = x << 16; }
	if ((x >> 24) == 0) { n = n + 8; x = x << 8; }
	if ((x >> 28) == 0) { n = n + 4; x = x << 4; }
	if ((x >> 30) == 0) { n = n + 2; x = x << 2; }
	n = n - (x >> 31);
	return 31 - n;
}
void Append_g(u32 runs, uchar **buffPPtr, uchar &offset)
{//runlength gamma
	uchar *ptr = *buffPPtr;
	uchar *ptrs = ptr;
	uchar off = offset;
	uchar bitsLen = (getBitsNum(runs) << 1) + 1;
	ptr += (off + bitsLen) >> 3;
	*buffPPtr = ptr;
	off = (off + bitsLen) & 0x7;
	offset = off;
	runs = runs << (8 - off);
	*ptr += runs & 0xff;
	if (ptr == ptrs)
		return;
	*(--ptr) += (runs >> 8) & 0xff;
	if (ptr == ptrs)
		return;
	*(--ptr) += (runs >> 16) & 0xff;
	if (ptr == ptrs)
		return;
	*(--ptr) += (runs >> 24) & 0xff;
	if (ptr == ptrs)
		return;
}

int compInc(std :: string file)
{
	int outsize = 11 * 1024 * 1024;;
	unsigned char *sBuff=new unsigned char[outsize];
	unsigned char outIndex=0;
	memset(sBuff,0,outsize);
	unsigned char *mBuff=sBuff;
	
	std :: ifstream fin;
	
	fin.open(file,std :: ios_base :: in);
	if(!fin.is_open())
	{
		std :: cout << "Can't open the file\n";
		exit(EXIT_FAILURE);
	}
	std :: string seq;
	int num = 0;
	char ch;
	while(fin.get(ch))
	{
		if(ch != '\n')
			seq += ch;
		else
		{
			num++;
			int diff = atoi(seq.c_str()) + 1;
			Append_g(diff, &mBuff, outIndex);
			seq = "";	
		}
	}
	unsigned int sBitLen=(mBuff-sBuff)*8+outIndex;
	/*
	需要存储字符数组sBuff以及其对应的比特位长度sBitLen 存储格式为<sBitLen，sBuff>
	int nBytes=sBitLens/8 + sBitLen%8?1:0;
	write(fd,&sBitLen,sizeof(unsigned int));
	write(fd,sBiff,nBytes);
	delete[] sBuff;（new - delete/ malloc - free）
	sBuff=NULL;
	*/
	unsigned long long int numbytes;
	if(sBitLen % 8 == 0)
		numbytes =  (sBitLen / 8);
	else
		numbytes =  (sBitLen / 8) + 1;
	FILE *fp;
	if((fp = fopen("D:\\BioStudy\\MyFastq\\MyFastq\\outputID\\cmpedInc.txt","wb+")) == NULL)
	{
		std :: cout << "Can't open the file\n";
		exit(EXIT_FAILURE);
	}
	/*
	for(int i = 0; i < 10; i++)
		{
			int j = sBuff[i];
			std :: cout << j << std :: endl;
	}
	*/
	numbytes = fwrite(sBuff,sizeof(sBuff[0]),numbytes,fp);
	delete [] sBuff;
	sBuff = NULL;
	int sizekb = numbytes / 1024 + 1;
	std :: cout << "Increment_GAMMA压缩后：" << sizekb << "KB\n";
	
	fin.close();
	return sizekb;
}


	
	