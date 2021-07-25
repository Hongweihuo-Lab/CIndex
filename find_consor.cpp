#include"find_consor.h"
#include<iostream>
#include<math.h>

std :: string cmpPermu(std :: string qvseq)
{
	int table[128] = {0};
	std :: string ans = "";
	for(int i = 0; i < qvseq.size(); i++)
		table[qvseq[i]]++;
	
	int C[128] = {0};
	//按频率排序
	for(int i = 1; i < 128; i++)
	{
		int key = table[i];
		int j = i - 1;
			while(j >= 0 && table[C[j]] < key)
			{
				C[j + 1] = C[j];
				j--;
			}
			C[j + 1] = i;
		
	}
	for(int i = 0; i < 128; i++)
	{
		while(table[C[i]] > 0 && ans.size() < qvseq.size())
		{
			ans += C[i];
			table[C[i]]--;
		}
	}
	return ans;
}

char getFch(int table[128])
{
	unsigned long freq = 0;
	char ch;
	for(int i = 0; i < 128; i++)
	{
		if(table[i] > freq)
		{
			freq = table[i];
			ch = i;
		}
	}
	return ch;
}
std :: string cmpConsor(std :: vector <std :: string> QV)
{ 
	
	std :: vector <std :: string> permutation;
	int maxLen = QV[0].size();
	for(int i = 0; i < QV.size(); i++)
	{
		if(QV[i].size() > maxLen)maxLen = QV[i].size();
		permutation.push_back(cmpPermu(QV[i]));
	}
	std :: string ans = "";
	char ch;
	unsigned int maxFreq = 0;
	for(int i = 0; i < maxLen; i++)
	{
		int chFreq[128] = {0};
		for(int j = 0; j < permutation.size(); j++)
		{
			if(i < permutation[j].size())
				chFreq[permutation[j][i]]++;
		}
		ch = getFch(chFreq);
		ans += ch;
	}
	return ans;
}

#if 0
std :: string cmpConsor(std :: vector <std :: string> QV)
{
	int qvlen = QV[0].size();
	//std :: cout << "qvlen = " << qvlen << std :: endl;
	//int qvlen = 35;
	int table[128] = {0};
	/*int ** count = new int* [qvlen];
	for(int i = 0; i < qvlen; i++)
		count[i] = new int [128];
	for(int i = 0; i < qvlen; i++)
	{
		for(int j = 0; j < 128; j++)
		{
			count[i][j] = 0;
		}
	}
	*/
	unsigned int totalFreq = 0;
	for(int i = 0; i < QV.size(); i++)
	{
		//if(QV[i].size() != QV[0].size())
			//std :: cout << "QV[i].size() = " << QV[i].size() << std :: endl;
		
		for(int j = 0; j < QV[i].size(); j++)
		{
			//count[j][QV[i][j]]++;
			table[QV[i][j]]++;
			totalFreq ++;
		}
	}
	/*
	for(int i = 0; i < 128; i++)
	{
		if(table[i] > 0)
		{
			printf("%c %d\n",i,table[i]);
		}
	}
	*/
	int C[128] = {0};
	//按频率排序
	for(int i = 1; i < 128; i++)
	{
		int key = table[i];
		int j = i - 1;
			while(j >= 0 && table[C[j]] < key)
			{
				C[j + 1] = C[j];
				j--;
			}
			C[j + 1] = i;
		
	}
	/*int csize = 0;
	for(int i = 0; i < 256; i++)
	{
		if(table[i]!=0)
			C[csize++]=i;
	}*/
	
	printf("按字符频率排名--C表:\n");
	for(int i = 0; i < 128; i++)
	{
		if(table[C[i]] > 0)
		{
			printf("%c %d %.2f\n",C[i],table[C[i]],(double)table[C[i]]/totalFreq);
		}
	}
	//按照百分比产生concensor
	std :: string temp = "";
	for(int i = 0; i < qvlen; i++) //此处qvlen可更改
	{
		if(table[C[i]] != 0)
	  {
		double percent = (double)table[C[i]]/totalFreq;
		int len = qvlen * percent;
		if(len < 1)
		{
			temp += C[i];
		}
		else
		{
			len = ceil((double)len);
			while(len && temp.size() < qvlen)
			{
				temp += C[i];
				len--;
			}
		}
		if(temp.size() == qvlen)
			break;
		}
	}
	
	/*
	for(int i = 0; i < qvlen; i++)
	{
		temp += maxfreq(count[i],193);
	}
	*/

	//std :: cout << "consor = " << temp << std :: endl;
	return temp;
}
#endif
char maxfreq(int *line, int len)
{
	unsigned char max = 0;
	int maxfreq = 0;
	for(int i = 0; i < 128; i++)
	{
		if(line[i] > maxfreq)
		{
			maxfreq = line[i];
			max = i;
		}
	}
	
	return max;
}

