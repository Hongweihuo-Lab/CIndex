#include"EliasCode.h"
/**
 * 从一串位向量的指定位置开始解码出一个gamma编码的数值 
 * @param &num 通过gamma编码解码出来的数值
 * @param **buffPPtr 指向待解码位向量某一具体字节的指针的指针
 * @param *offset 指向某一具体待解码字节的偏移量 
 */
void elisGammaDecode(int &num,unsigned char **buffPPtr,unsigned char* offset)
{
	int bisLen=0;
	unsigned char *ptr=*buffPPtr;
	unsigned char off=*offset;
	while (!(*ptr&(1<<(7-off))))
	{
		if (++off==8)
		{
			off=0;
			ptr++;
		}
		bisLen++;
	}

	unsigned sum=0;
	int i;

    for (i=0;i<=bisLen;i++)
    {
        sum=(sum<<1) + ((*ptr&(1<<(7-off)))?1:0);
        if (++off==8)
        {
            off=0;
            ptr++;
        }
    }

	*buffPPtr=ptr;
	*offset=off;
	num=sum;
}
