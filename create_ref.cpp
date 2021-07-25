#include"create_ref.h"
#include<vector>
#include<string>
#include<fstream>
#include<math.h>
#include"create_newLevel.h"
int getmin(int x, int y)
{
	return (x < y) ? x : y;
}
/*int score(std :: string str1, std :: string str2)
{
	int count[2][128] = {0};
	for(int i = 0; i < str1.size(); i++)
	{
		count[0][str1[i]]++;
	}
	for(int i = 0; i < str2.size(); i++)
	{
		count[1][str2[i]]++;
	}
	int soc = 0;
	for(int i = 0; i < 128; i++)
	{
		soc += getmin(count[0][i],count[1][i]);
	}
	return soc;
}
*/
//use cos(str1,str2) to determine the similarity of two strings
double cos(int vec[2][128],int size)
{
	int top = 0;
	double bottom0 = 0;
	double bottom1 = 0;
	for(int i = 0; i < size; i++)
	{
		top += (vec[0][i] * vec[1][i]);
		bottom0 += (vec[0][i] * vec[0][i]);
		bottom1 += (vec[1][i] * vec[1][i]);
	}
	double soc = top/(sqrt(bottom0) * sqrt(bottom1));
	return soc;
}
double score(std :: string str1, std :: string str2)
{
	int count[2][128] = {0};
	
	for(int i = 0; i < str1.size(); i++)
	{
		count[0][str1[i]]++;
	}
	bool flag = true;
	for(int i = 0; i < str2.size(); i++)
	{
		if(str2[i] == ' ')
		{
			flag = false;
			continue;
		}
		if(!flag)
			count[1][str2[i]]++;
	}
	double soc = cos(count,128);
	return soc;
}

void createRef(std :: vector<std :: string> QV,int *C,int num, std :: vector <std :: string> &ref) //num为参考序列个数
{
	int reflen = QV[0].size();  //参考序列的长度与原序列长度一致
	for(int i = 0; i < num; i++)
	{
		std :: string temp = "";
		for(int j = 0; j < reflen; j++)
		{
			temp += C[i];
		}
		ref.push_back(temp);
	}
	return;
}
void level_1(std :: string Filename, std :: vector<std :: string> &QV,std :: vector <std :: string> ref,int part)
{
	
	int *refcount = new int [ref.size()]; 
	for(int i = 0; i < ref.size(); i++)
		refcount[i] = 0;
	int qvlen = QV[0].size();	
	std :: vector<std :: vector <std :: string>> cluster;
	for(int i = 0; i < ref.size(); i++)
	{
		std :: vector <std :: string> neighbor;
		cluster.push_back(neighbor);
	}

	std :: vector<std :: string>::iterator iter = QV.begin();
	
	while(iter != QV.end())
	{
		bool flag = true;
		for(int j = 0; j < ref.size(); j++)
		{
			double sco = score(ref[j], *iter);
			if(sco >= COS_30)
			{
				refcount[j]++;
				cluster[j].push_back(*iter);
				iter = QV.erase(iter); //移除，应该用erase
				flag = false;
				break;
			}
			
		}
		if(flag)
			iter++;
	}
	for(int i = 0; i < cluster.size(); i++)
	{
		if(cluster[i].size() > 0)
		{
			
			std :: ofstream fout; //写入level1_.txt文本
			std :: string temp = (Filename + "32_");
			char t = ref[i][0];
			std :: string str1 = num2str(t);
			temp += str1;
			temp += ".txt";
			fout.open(temp,std :: ios_base :: out | std :: ios_base :: app);
			if(!fout.is_open())
			{
				std :: cout << "Can't open the" << temp << "\n";
				exit(EXIT_FAILURE);
			}
			
			fout << part << std :: endl;
			
			for(int j = 0; j < cluster[i].size(); j++)
			{
				/*unsigned char* ans = runlen(cluster[i][j], ref[i][0]);
				fout.write((char *)ans,strlen((char *)ans)+1);
				fout << std :: endl;
				delete [] ans;
				ans = NULL;*/
				fout << cluster[i][j] << std :: endl;
				
			}
		}
	}
	delete [] refcount;
	return;
}
