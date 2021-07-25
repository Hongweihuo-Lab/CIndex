#include"EliasCode.h"
/**
 * ��һ��λ������ָ��λ�ÿ�ʼ�����һ��gamma�������ֵ 
 * @param &num ͨ��gamma��������������ֵ
 * @param **buffPPtr ָ�������λ����ĳһ�����ֽڵ�ָ���ָ��
 * @param *offset ָ��ĳһ����������ֽڵ�ƫ���� 
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
