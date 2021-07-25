#include<string>
#include<fstream>
#include<vector>
#include<math.h>
#include"create_newLevel.h"
#include"create_ref.h"
#include"find_consor.h"
#include"DNAcmp.h"

std :: string num2str(int level)
{
	std :: string temp = "";
	std :: string ans = "";
	if(level == 0)
		return "0";
	while(level)
	{
		temp+=(level % 10 + 48);
		level /= 10;
	}
	for(int i = temp.size() - 1; i >= 0; i--)
	{
		ans += temp[i];
	}
	return ans;
}

unsigned char * runlen(std :: string qvseq,char most) //1
{
	int size = qvseq.size();
	unsigned char *ans = new unsigned char [size + 1];
	unsigned char len = 0;
	unsigned char base = 1 << 7;
	
	int blocknum = 0;
	bool flag = true;
	for(int i = 0; i < qvseq.size();)
	{
		
		if(flag)
		{
			ans[blocknum++] = qvseq[i];
			if(qvseq[i] == ' ')
				flag = false;
			i++;
			continue;
		}
		if(!flag)
		{
			if(qvseq[i] == most)
			{
				len = 1;
				int j = i + 1;
				for(;j < qvseq.size(); j++)
				{
					if(most == qvseq[j])
						len++;
					else break;
				}
				i = j;
				ans[blocknum++] = (base + len);
			}
			else
			{
				ans[blocknum++] = qvseq[i];
				i++;
			}
		}
	}
	ans[blocknum] = '\0';
	return ans;
}
std :: string de_runlen(std :: string ans,char most)
{
	unsigned char base = (1 << 7);
	std :: string str = "";

	for(int i = 0; i < ans.size() - 1; i++)
	{
		unsigned char ch = ans[i];
		int int_ch = ch;
		if(int_ch > base)
		{
			int len = int_ch - base;
			for(int j = 0; j < len; j++)
			{
				str += most;
			}
		}
		else
			str += ans[i];
	}
	return str;
}
std :: string rl_2bytes(std :: string seq)//2
{
	std :: string ans = "";
	unsigned char len;
	unsigned char c;
	unsigned char base = (1 << 7);
	for(int i = 0; i < seq.size();)
	{
		c = seq[i];
		len = 1;
		int j = i + 1;
		for(;j < seq.size(); j++)
		{
			if(c == seq[j])
				len++;
			else break;
		}
		i = j;
		if(len == 1)
			ans += c;
		else
		{
			ans += (base + c);
			ans += len;
		}
	}
	return ans;
}

std :: string rl_triple(std :: string seq,char most)//3  len全部加1,0没法存 ，还需好方法
{
	std :: string ans = "";
	for(int i = 0; i < seq.size(); )
	{
		int len1 = 1; //run-length of most
		int len2 = 1; // run-length of others
		while(i < seq.size() && seq[i] == most)
		{
			len1++;
			i++;
		}
		while(i < seq.size()-1 && seq[i] == seq[i+1])
		{
			len2++;
			i++;
		}
		ans += len1;
		ans += seq[i];
		ans += len2;
		i++;
	}
	return ans;
}


#if 0
std :: string choose(std :: string qvseq,char most)
{
	std :: vector<std :: string> ans ;
	ans.push_back(qvseq);
	//ans.push_back(runlen(qvseq));
	ans.push_back(rl_2bytes(qvseq));
	ans.push_back(rl_triple(qvseq,most));
	
	int method = 0;
	int min_len = qvseq.size();

	for(int i = 0; i < ans.size(); i++)
	{
		if(ans[i].size() < min_len)
		{
			method = i;
			min_len = ans[i].size();
		}
	}
	ans[method].insert(ans[method].begin(),('0' + method)); //save the method used 
	return ans[method];
}

void create_newlevel(std :: vector<std :: string> &QV, double gmd,int level,int originalSize,char most)
{

	std :: string consor = cmpConsor(QV);
	int qvlen = QV[0].size();
	std :: ofstream fout; 
	std :: string filename = "D:\\BioStudy\\MyFastq\\MyFastq\\outputData\\level";
	filename += num2str(level);
	filename += ".txt";
	std :: cout << "create " << filename << std :: endl;
	std :: cout << "consor = " << consor << std :: endl;

	fout.open(filename,std :: ios_base :: out | std :: ios_base :: app);
    if(!fout.is_open())
	 {
		   std :: cout << "Can't open the " << filename << "\n";
		   exit(EXIT_FAILURE);
	 }

	//记录每个level的consor
	
	std :: string Rec_consor = "D:\\BioStudy\\MyFastq\\MyFastq\\outputData\\cons_level";
	Rec_consor += num2str(level);
	std :: ofstream fout2;
	fout2.open(Rec_consor,std :: ios_base :: out | std :: ios_base :: app);
    if(!fout2.is_open())
	 {
		   std :: cout << "Can't open the " << Rec_consor << "\n";
		   exit(EXIT_FAILURE);
	 }
	fout2 << consor << std :: endl;
	fout2.close();
	
	if(QV.size() > originalSize * 0.01)
	{
		std :: vector<std :: string>::iterator iter;
		int y = 0;
		for(iter = QV.begin(); iter != QV.end();)
		{
			double sco = score(consor, *iter);
				//if(sco <= (qvlen * gmd))
			    if(sco <= gmd)
				{
					//if(sco >= COS_20)
						//runlen(*iter);
					
					std :: string strrun = choose(*iter,most);
					fout << y << ":"<< strrun << std :: endl;
					iter = QV.erase(iter);
					y++;
				}
				else
				{
					iter++;
				}
		}
		if((QV.size() <= originalSize * 0.01 || gmd >= COS_15) && QV.size() > 0 )
	    {
		   std :: vector<std :: string>::iterator iter;
		   std :: cout << "该部分最后剩："<<QV.size()<<std :: endl;
		   int y = 0;
		   for(iter = QV.begin(); iter != QV.end();)
		   {
			  
			  std :: string strrun = choose(*iter,most);
			  fout << y << ":"<< strrun << std :: endl;
			  iter = QV.erase(iter);
			  y++;	
		   }
		   std :: cout << "最后："<<QV.size()<<std :: endl;
		   QV.clear();
		   return;
	    }
	}
	
	else
	{
		if(QV.size() > 0)
		{
		
		std :: vector<std :: string>::iterator iter;
		std :: cout << "该部分最后剩：" << QV.size() << std :: endl;
		int y = 0;
		for(iter = QV.begin(); iter != QV.end();)
		{
			  std :: string strrun = choose(*iter,most);
			  fout << y << ":"<< strrun << std :: endl;
			  iter = QV.erase(iter);
			  y++;	
		}
		std :: cout << "最后："<<QV.size()<<std :: endl;
		}
		QV.clear();
	}
	//std :: cout << "QV.size() = " << QV.size() << std :: endl;
	return;
}
#endif

char getMostFreqCh(std :: string str)
{
	int table[128] = {0};
	bool flag = true;
	for(int i = 0; i < str.size(); i++)
	{
		if(str[i] == ' ')
		{
			flag = false;
			continue;
		}
		if(!flag)
			table[str[i]]++;
	}
	int max = 0;
	char ch;
	for(int i = 0; i < 128; i++)
	{
		if(table[i] > max)
		{ch = i;max = table[i];}
	}
	return ch;
}
char get2rdMostCh(std :: string str,char most)
{
	int table[128] = {0};
	bool flag = true;
	for(int i = 0; i < str.size(); i++)
	{
		if(str[i] == ' ')
		{
			flag = false;
			continue;
		}
		if(!flag && str[i] != most)
			table[str[i]]++;
	}
	int max = 0;
	char ch;
	for(int i = 0; i < 128; i++)
	{
		if(table[i] > max)
		{ch = i;max = table[i];}
	}
	return ch;
}
std :: string createStr(int len, char ch)
{
	std :: string ans = "";
	for(int i = 0; i < len; i++)
		ans += ch;
	return ans;
}
void set1(unsigned char* mask,int i) //set ith to 1
{
	int block = i / 8;
	int offset = i % 8;
	unsigned char ch = 1 << (8 - (offset + 1));
	mask[block] |= ch;
}

unsigned char* trim(std :: string &str, char most)
{
	std :: string ans = "";
	double len = str.size();
	int size = ceil(len / 8);
	unsigned char *mask = new unsigned char [size];
	for(int i = 0; i < size; i++)
		mask[i] = 0;
	for(int i = 0; i < len; i++)
	{
		if(str[i] == most)
		{
			set1(mask,i);
		}
		else
			ans += str[i];
	}
	str = ans;
	return mask;
}
int countTable(std :: string str)
{
	int table[128] = {0};
	bool flag = true;
	for(int i = 0; i < str.size();i++)
	{
		if(flag)
		{
			if(str[i] == ' ')
				flag = false;
			continue;
		}
		else
		table[str[i]]++;
		
	}
	int num = 0;
	for(int i = 0; i < 128; i++)
	{
		if(table[i])num++;
	}
	return num;
}
void create_newlevel(std :: vector<int> key, std :: vector<std :: string> &QV, const std :: string filename, int &level, int qvnum,int part, bool part_flag)
{
	int time = 0;
	std :: vector<std :: vector <std :: string>> cluster(128);
	std :: vector<std :: string>::iterator iter;

	unsigned long long numi = 0;
	int maxnum = 0;
	for(iter = QV.begin();iter != QV.end();iter++)  // iter++!!!
	{
		int keyi = key[numi++];
		if(keyi > maxnum)maxnum = keyi;
		cluster[keyi].push_back(*iter);
	}
	QV.clear();
	std :: cout << "maxnum = " << maxnum << std :: endl;
	int end = maxnum / 3;
	for(int i = 1; i < end; i++)
	{
		if(cluster[i].size()>0)
		{
			
			std :: ofstream fout; 
			std :: string temp = "";
			temp += filename;
			//temp += num2str(i);
			temp += num2str(0);
			temp += ".txt";
			fout.open(temp,std :: ios_base :: out | std :: ios_base :: app);
			if(!fout.is_open())
			{
			    std :: cout << "Can't open the " << filename << "\n";
				exit(EXIT_FAILURE);
			}

			if(!part_flag)
				fout<< part << std :: endl;

			for(iter = cluster[i].begin(); iter != cluster[i].end(); iter++)//iter++!!
			{			
				fout << *iter << std :: endl;
			}
			cluster[i].clear();
			fout.close();
		}
	}
#if 1
	//int end2 = maxnum / 2;
	std :: string fileout;
	for(int i = end; i < 128; i++)
	{
		/*if(i < end2 && cluster[i].size()>0)
		{
			
			std :: ofstream fout; 
			std :: string temp = "";
			temp += filename;
			
			temp += num2str(part);
			temp += "_";
			
			temp += num2str(i);
			temp += ".txt";
			fout.open(temp,std :: ios_base :: out | std :: ios_base :: trunc);
			if(!fout.is_open())
			{
			    std :: cout << "Can't open the " << filename << "\n";
				exit(EXIT_FAILURE);
			}

			//if(!part_flag)
			fout<< part << std :: endl;

			for(iter = cluster[i].begin(); iter != cluster[i].end();iter++) // iter++!!
			{		
				fout << *iter << std :: endl;
			}
			cluster[i].clear();
			fout.close();
		}*/
	   if(cluster[i].size()> 0)
		{
			
			std :: ofstream fout2; 
			std :: string temp = "";
			temp += filename;
			//temp += num2str(part);
			temp += num2str(1);
			//temp += "_";
			//temp += num2str(end);
			temp += ".txt";
			fileout = temp;
			fout2.open(temp,std :: ios_base :: out | std :: ios_base :: app);
			if(!fout2.is_open())
			{
				std :: cout << "Can't open the " << filename << "\n";
				exit(EXIT_FAILURE);
			}

			if(!part_flag)
				fout2 << part << std :: endl;

			for(iter = cluster[i].begin(); iter != cluster[i].end();iter++) //iter++!
			{
				fout2 << *iter << std :: endl;
			}
			cluster[i].clear();
			fout2.close();
			
		}	
	}
#endif
#if 0
	//int end2 = 20;
	std :: ofstream fout; 
	std :: string temp = "";
	temp += filename;
	temp += num2str(part);
	temp += "_";
	temp += num2str(end2);
	temp += ".txt";
	fout.open(temp,std :: ios_base :: out | std :: ios_base :: trunc);
	if(!fout.is_open())
	{
		std :: cout << "Can't open the " << filename << "\n";
		exit(EXIT_FAILURE);
	}
	for(int i = end2; i < 128; i++)
	{
		if(cluster[i].size()>0)
		{
			for(iter = cluster[i].begin(); iter != cluster[i].end(); )
			{
							
				fout << *iter << std :: endl;
				//iter = cluster[i].erase(iter);
			}
			cluster[i].clear();
		}	
	}
	fout.close();
	
#endif	
// old fashion
#if 0 
	
	for(iter = QV.begin(); iter != QV.end();)
	{
		int num = getMostFreqCh(*iter);
		cluster[num].push_back(*iter);
		iter = QV.erase(iter);
	}
	for(int i = 0; i < 128; i++)
	{
		if(cluster[i].size() > 0)
		{
			
			std :: string ref = createStr(qvlen,i);

			std :: vector<std :: vector <std :: string>> cluster2(128);
			for(iter = cluster[i].begin(); iter != cluster[i].end();)
			{
				if(score(ref,*iter) >= COS_45)
				{
					int num = i;
					cluster2[num].push_back(*iter);
					iter = cluster[i].erase(iter);
				}
				else
				{
					int num = get2rdMostCh(*iter,i);
					cluster2[num].push_back(*iter);
					iter = cluster[i].erase(iter);
				}
			}
			bool flag = true;
			for(int k = 0; k < 128; k++)
			{
				if(cluster2[k].size() > 0 && cluster2[k].size() > thread)
				{
					std :: ofstream fout; 
					std :: string temp = "";
					temp += filename;
					temp += num2str(i);
					temp += "_";
					temp += num2str(k);
					temp += ".txt";
					fout.open(temp,std :: ios_base :: out | std :: ios_base :: app);
					if(!fout.is_open())
					{
						 std :: cout << "Can't open the " << filename << "\n";
						 exit(EXIT_FAILURE);
					}
					if(!part_flag)
						fout<< part << std :: endl;
		            if(k == i)
					{
						
						for(iter = cluster2[k].begin(); iter != cluster2[k].end(); )
						{
							/*unsigned char* ans = runlen(*iter, i);
							fout.write((char *)ans,strlen((char *)ans) + 1);
							fout << std :: endl;
							delete [] ans;
							ans = NULL;
							iter = cluster2[k].erase(iter);*/
							fout << *iter << std :: endl;
							iter = cluster2[k].erase(iter);

						}
						fout.close();
						continue;
					}
					
					for(iter = cluster2[k].begin(); iter != cluster2[k].end();)
					{
						fout << *iter << std :: endl;
						iter = cluster2[k].erase(iter);
					}
					fout.close();
	
				}
				else if(cluster2[k].size() > 0 && cluster2[k].size() <= thread)
					flag = false;
			}


			if(!flag)
			{
				    std :: ofstream fout; 
					std :: string temp = "";
					temp += filename;
					temp += num2str(i);
					temp += ".txt";
					
					fout.open(temp,std :: ios_base :: out | std :: ios_base :: app);
					if(!fout.is_open())
					{
						 std :: cout << "Can't open the " << filename << "\n";
						 exit(EXIT_FAILURE);
					}
					if(!part_flag)
						fout<< part << std :: endl;

				    for(int k = 0; k < 128; k++)
				    {
						if(cluster2[k].size() > 0)
						{   
							for(iter = cluster2[k].begin(); iter != cluster2[k].end();)
							{
								fout << *iter << std :: endl;
								iter = cluster2[k].erase(iter);
							}
						}
				   }
					fout.close();
			 }
		}
	}
#endif

	return;
}