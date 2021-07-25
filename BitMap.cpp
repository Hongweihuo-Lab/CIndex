/*============================================
# Filename: BitMap.cpp
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description:
=============================================*/
#include"BitMap.h"
// #include"select.h"
#include<math.h>
#include<iostream>
#include <iomanip>
#define FILEANDLINE cout<<__FILE__<<":"<<__LINE__<<setw(20);
using namespace std;
#define lookuptable
//#define EFTABLE

const int FixLenBit = 8;
inline u64 getValue64(u64 *temp, u64 index)
{
	int words = index >> 6;
	u64 offset = index & 0x3f;
	if (offset)
	{
		return (temp[words] << offset) + (temp[words + 1] >> (64 - offset));
	}
	else
	{
		return temp[words];
	}
}
void printBits_u64(u64 x)
{
	u64 tmp = (1ULL << 63);
	while (tmp)
	{
		if (x & tmp)
		{
			cout << "1";
		}
		else
		{
			cout << "0";
		}
		tmp = (tmp >> 1);
	}
}
inline uint32_t select(uint64_t x, uint32_t i) //count from right //now add new
{
	if (i == 0)
		return 0;
}
uint32_t selectlzero(uint64_t x, uint32_t i) //get the i 0 pos from x ,count left for 64bits
{
	// if (x == 935622822586220544&& i==6)
	// FILEANDLINE;
	if(popcnt(x)>sizeof(uint64_t)*8-i)//in x there less 0 cnt than i
		return 65;
	auto zzz = select(~x, i);
	return zzz;
}
u32 firstZeroFromLeft(u64 n)
{
#ifdef __SSE4_2__
	// cout << __builtin_clzl(n)<<endl;
	return __builtin_clzl(n);
#endif
	int result;
	if (n & 0xffffffff00000000)
	{
		result += 32;
		n >>= 32;
	}
	if (n & 0x00000000ffff0000)
	{
		result += 16;
		n >>= 16;
	}
	if (n & 0x000000000000ff00)
	{
		result += 8;
		n >>= 8;
	}
	if (n & 0x00000000000000f0)
	{
		result += 4;
		n >>= 4;
	}
	if (n & 0x000000000000000c)
	{
		result += 2;
		n >>= 2;
	}
	if (n & 0x0000000000000002)
	{
		result += 1;
		n >>= 1;
	}
}
int BitMap::findzero(u64 *bits,int len,int k){
	int loop=len/64;
	int over=len%64;
	int result=0;
	int i=0;
	for(i=0;i<loop;i++){
		int temp=popcnt(~bits[i]);
		if(temp<k){
			k-=temp;
			result+=64;
		}else{
			result+=select_in_word_big(~bits[i],k);
			return result;
		}
	}
	if(over>0){
		int temp=select_in_word_big(~bits[i],k);
		if(temp<0||temp>over){
			return -1;
		}
		else{
			return result+temp;
		}
	}
}

inline int popcnt(unsigned long long int x)
{
#ifdef __SSE4_2__
	// cout<<"sse";
	return __builtin_popcountll(x);
#else
#ifdef POPCOUNT_TL
	return lt_cnt[x & 0xFFULL] + lt_cnt[(x >> 8) & 0xFFULL] +
		   lt_cnt[(x >> 16) & 0xFFULL] + lt_cnt[(x >> 24) & 0xFFULL] +
		   lt_cnt[(x >> 32) & 0xFFULL] + lt_cnt[(x >> 40) & 0xFFULL] +
		   lt_cnt[(x >> 48) & 0xFFULL] + lt_cnt[(x >> 56) & 0xFFULL];
#else
	x = x - ((x >> 1) & 0x5555555555555555ull);
	x = (x & 0x3333333333333333ull) + ((x >> 2) & 0x3333333333333333ull);
	x = (x + (x >> 4)) & 0x0f0f0f0f0f0f0f0full;
	return (0x0101010101010101ull * x >> 56);
#endif
#endif
}

vector<int> BitMap::ConvertRLtopos(int *runs,int num,int firstbit,int &bit){
	int k=0;
	int flag=firstbit;
	vector<int> pos_of_ones,pos_of_zeros;
	for(int i=0;i<num;i++){
		for(int j=0;j<runs[i];j++){
			if(flag==1){
				pos_of_ones.push_back(k);
			}else{
				pos_of_zeros.push_back(k);
			}
			k++;
		}
		flag=1-flag;
	}
	if(pos_of_zeros.size()<pos_of_ones.size()){
		bit=0;
		return pos_of_zeros;
	}else{
		bit=1;
		return pos_of_ones;
	}
}
int* BitMap::ConvertRLtogap(vector<int> pos,int low){
	size_t num=pos.size();
	int *gap=new int[num];
	for(int i=num-1;i>0;i--){
		gap[i]=(pos[i]>>low)-(pos[i-1]>>low);
	}
	gap[0]=pos[0]>>low;
	return gap;
}

int blog64(u64 x)
{
	int ans = 0;
	while(x>0)
	{
		ans++;
		x=x>>1;
	}
	return ans;
}

u32 blog(i64 x)
{
	int ans = 0;
	while(x>0)
	{
		ans++;
		x=x>>1;
	}
	return ans;
}

BitMap::BitMap(unsigned long long int * bitbuff,i64 bit_len,int level,int method,int block_size,unsigned char label,uchar ** tables)
{
	this->data = bitbuff;
	this->bitLen = bit_len;
	this->memorysize = 0;
	this->level = level;
	this->block_size = block_size;
	this->block_width = blog(block_size);
	this->super_block_size = 16*block_size;
	this->label = label;
	left=NULL;
	right=NULL;
	superblock =NULL;
	block=NULL;
	coding_style=NULL;
	R=NULL;
	Z=NULL;
	coding_tpye =method;
	if(data!=NULL)
	{
		this->Z = tables[0];
		this->R = tables[1];
		this->R1 = tables[2];
		this->R3 = tables[3];
		if (coding_tpye !=1){//rl code
			Coding();
		}
		else{
			Coding2();
		}
		buff =NULL;
	}
}
i64 BitMap::SizeInByte()
{

	i64 size = 0;
	if(data!=NULL)
	{
		//return bitLen/8;
		size+= superblock->GetMemorySize();
		size+= block->GetMemorySize();
		size+= coding_style->GetMemorySize();
		size+= memorysize;
	}
	return size;
}
i64 BitMap::SizeInSuperblock()
{
	int size = 0;
	if(data!=NULL)
		size+= superblock->GetMemorySize();
    return size;
}
i64 BitMap::SizeInblock()
{
	int size = 0;
	if(data!=NULL)
		size+= block->GetMemorySize();
    return size;
}
i64 BitMap::SizeIncodesytle()
{
	int size = 0;
	if(data!=NULL)
		size+= coding_style->GetMemorySize();
    return size;
}
i64 BitMap::SizeInMemory()
{
	int size = 0;
	if(data!=NULL)
		size+= memorysize;
    return size;
}

i64 BitMap::ALLcount 	   = 0;
// i64 BitMap::ALL1	   = 0;
// i64 BitMap::RL0 	   = 0;
i64 BitMap::Othercount   = 0;
i64 BitMap::Plaincount = 0;
i64 BitMap::EFcount = 0;
i64 BitMap::RLcount = 0;
///////////////////////////
i64 BitMap::plainSize = 0;
i64 BitMap::FixSize = 0;
i64 BitMap::RLSize = 0;
i64 BitMap::EFSize = 0;
i32 BitMap::Block_size = 0;
i32 BitMap::superblock_size = 0;
map<i64,i64> BitMap::mapruns;
map<i64,i64> BitMap::noteruns;
vector<int> BitMap::ConvertGapTODiv(vector<int> gaps,i64& firstbit)
{
    vector<int> divs;
	if(gaps.size()<=0)
		return gaps;
    divs.push_back(gaps[0]);
    for(int i = 0 ;i<gaps.size()-1;i++){
        divs.push_back(gaps[i+1] - gaps[i]);
    }
    return divs;
}
vector<int> BitMap::ConvertRLtoDiv(int * run,int len,i64& firstbit)
{
	return ConvertGapTODiv(ConvertRLtoGap(run,len,firstbit),firstbit);
}
vector<int> BitMap::ConvertRLtoGap(int * run,int len,i64& firstbit)
{
    vector<int> gaps1,gaps2;
    int curpos = 0;
    int curgap = 0;
    // for(auto i : values){
    for(int i = 0;i<len;i++){
        for(int j = 0 ;j< run[i];j++){
            if(curgap%2 ==0)
                gaps1.push_back(curpos++);
            else
                gaps2.push_back(curpos++);

        }
        curgap++;
    }

    if(gaps1.size()>gaps2.size()){
		firstbit = 1 - firstbit;
		return gaps2;
	}
    else{
        return gaps1;
	}

}
void BitMap::Coding()
{
	int u64Len = 0;
	if (bitLen % 64 == 0)
		u64Len = bitLen / 64;
	else
		u64Len = bitLen / 64 + 1;
	u64 *temp = new u64[u64Len]; //to save the compressed string temporarily
	memset(temp, 0, u64Len * 8);

	i64 index = 0;

	i64 step1 = super_block_size;//supersize
	i64 step2 = block_size;//bsize
	superblock = new InArray(2*(bitLen/step1)+6,log2(bitLen)+1);//need to add one more block for last block
	block      = new InArray(2*(bitLen/step2)+6,blog(step1-step2));//need to add one more block for last block
	coding_style = new InArray(bitLen / block_size + 1, 2);

	i64 rank =0;
	i64 space = 0;
	i64 bits = 0;
	i64 firstbit;
	int rl_g = 0;
	int runs = 0;
	int bit = 0;
	int *runs_tmp = new int[block_size]; //save the run-length
	i64 k = 0;
	i64 index2 = 0;
	i64 pre_rank=0;
	i64 pre_space =0 ;
	superblock->SetValue(0,0);
	superblock->SetValue(1,0);
	block->SetValue(0,0);
	block->SetValue(1,0);
	while (index < bitLen)
	{
		if (index == bitLen)
			break;
		rl_g = 0;
		bits = 0;
		firstbit = 0;
		runs = 0;

		firstbit = GetBit(data, index);
		memset(runs_tmp, 0, block_size * sizeof(int));
		k = 0;
		runs = 0;
		while (bits < block_size && index < bitLen)
		{

			runs = GetRuns(data, index, bit);
			bits = bits + runs;
			if(bit ==1)
				rank=rank+runs;
			runs_tmp[k] = runs;
			k++;
		}

		if (bits > block_size)
		{ 
			int step = 0;
			index = index - (bits - block_size);
			step = block_size + runs - bits;
			if(bit ==1)
				rank = rank -runs+step;
			runs_tmp[k - 1] = step;
		}

		for (int i = 0; i < k; i++)
			rl_g = rl_g + 2 * blog(runs_tmp[i]) - 1; //all the run-length

		int thred = 20; //yuzhi
		int len = min(rl_g, block_size - thred);
		if (len == (block_size - thred) || index == bitLen)//plain
		{ 
			Plaincount++;
			coding_style->SetValue((index - 1) / block_size, 2);
			int j = 0;
			int num = 0;
			if (index == bitLen)
			{
				space = space + bits;
				j = (index - bits) / 64;
				num = (bits % 64) ? bits / 64 + 1 : bits / 64;
			}
			else
			{
				space = space + block_size;
				j = (index - block_size) / 64;
				num = block_size / 64;
			}
			for (int kk = 0; kk < num; kk++, j++)
				BitCopy(temp, index2, data[j]);
		}
		else if (k == 1) //ALL0 and ALL1
		{
			ALLcount++;
			if (firstbit == 0)
			{

				coding_style->SetValue((index - 1) / block_size, 3);
			}
			else
			{

				coding_style->SetValue((index - 1) / block_size, 3);
			}
			space = space + 0;
		}
		else
		{ //rl_gamma

			Othercount++;
			if (firstbit == 0){
				coding_style->SetValue((index - 1) / block_size, 0);
			}
			else{
				coding_style->SetValue((index - 1) / block_size, 1);
			}
			space = space + rl_g;
			for (int i = 0; i < k; i++)
			{
				Append_g(temp, index2, runs_tmp[i]);
			}
		}
		if(index % step1 == 0)
		{
			pre_rank = rank;
			superblock->SetValue(2 * (index / step1), pre_rank);

			pre_space = space;
			superblock->SetValue(2 * (index / step1) + 1, pre_space);
		}
		if (index % step2 == 0)
		{
			block->SetValue(2 * (index / step2), rank - pre_rank);
			// cout<<rank - pre_rank<<endl;
			block->SetValue(2 * (index / step2) + 1, space - pre_space);
		}
	}

	delete[] runs_tmp;
	int u64_len_real = 0; //the compressed size of the wavelettree node
	if (space % 64 == 0)
		u64_len_real = space / 64 + 1;
	else
		u64_len_real = space / 64 + 1 + 1;

	this->memorysize = u64_len_real * 8;
	delete[] data;
	data = new u64[u64_len_real];
	memset(data, 0, u64_len_real * 8);
	memcpy(data, temp, (u64_len_real - 1) * 8);
	delete[] temp;
}
void BitMap::BitCopy1(u64 * temp,i64 &index,u64*data,i64 index1,int len)
{
	while(len>=64){
		u64 value=GetBits(data,index1,64);
		BitCopy(temp,index,value);
		len-=64;
		index1+=64;
	}
	if(len!=0){
		u64 value=GetBits(data,index1,len);
		if(index%64!=0){
			int first = index % 64;
			if(first+len<65){
				int overloop=64-(first+len);
				value=(value<<overloop);
				temp[index/64]=(temp[index/64]|value);
			}
			else{
				first=64-first;
				int second=len-first;
				int third=64-second;
				temp[index/64]=(temp[index/64]|(value>>second));
				temp[index/64+1]=temp[index/64+1]|(value<<third);
			}
		}
		else{
			int overloop=64-len;
			value=(value<<overloop);
			temp[index/64]=value;
		}
		index = index+len;
	}
}
void BitMap::Coding2()
{
	i64 u64Len = 0;
	if(bitLen%64 == 0)
		u64Len = bitLen/64;
	else
		u64Len = bitLen/64+1;
	u64 * temp = new u64[u64Len];
	memset(temp,0,u64Len*8);

	i64 index = 0;
	u64 step1 = super_block_size;
	u64 step2 = block_size;
	superblock = new InArray(2*(bitLen/step1)+4,blog(bitLen)+1);
	block      = new InArray(2*(bitLen/step2)+4,blog(step1-step2));
	coding_style      = new InArray(bitLen/step2+1,3);

	u64 rank=0;
	u64 space=0;
	u64 palinspace=0;
	u64 rlgspace=0;
	u64 bits =0;
	u64 firstbit;
	int rl_g=0;
	int runs = 0;
	int bit=0;
	int * runs_tmp = new int[block_size];
	u64 k=0;
	i64 index2=0;

	u64 pre_rank=0;
	u64 pre_space =0 ;
	superblock->SetValue(0,0);
	superblock->SetValue(1,0);
	block->SetValue(0,0);
	block->SetValue(1,0);

	while(index < bitLen)
	{
		if(index == bitLen)
			break;
		rl_g = 0;
		bits = 0;
		firstbit = 0;
		runs = 0;
		firstbit = GetBit(data,index);
		memset(runs_tmp,0,block_size*4);
		k=0;
 		runs=0;
		while(bits < block_size && index < bitLen)
		{

			runs = GetRuns(data,index,bit);
			bits = bits +runs;
			if(bit ==1)
				rank=rank+runs;
			runs_tmp[k] = runs;
			k++;
		}
		
		if(bits > block_size)
		{
			int step =0;
			index = index -(bits - block_size);
			step = block_size+runs-bits;
			if(bit ==1)
				rank = rank -runs+step;
			runs_tmp[k-1] = step;
		}
		for(int i=0;i<k;i++){
			rl_g = rl_g + 2*blog(runs_tmp[i])-1;
		}
		int codestyle=0;
		int EFLen=block_size;
		int *gap;
		vector<int> pos;
		int low=0;
		int NumOfElem=0;
		int *lowvalue;
		int eftype=0;
		if(k!=1){
			pos=ConvertRLtopos(runs_tmp,k,firstbit,eftype);
			NumOfElem=pos.size();
			low=blog(block_size/NumOfElem)-1;
			low=0>low?0:low;
			gap=ConvertRLtogap(pos,low);
			lowvalue=new int[NumOfElem];
			for(int i=0;i<NumOfElem;i++){
				lowvalue[i]=pos[i]&((1<<low)-1);
			}
			EFLen=NumOfElem+(pos[NumOfElem-1]>>low)+NumOfElem*low;
		}
		int thred=20;
		int len = min(rl_g,block_size-thred);
		codestyle=EFLen<len?1:0;
		if(len == (block_size-thred) || index == bitLen)//plain
		{
			plain++;
			coding_style->SetValue((index-1)/block_size,2);
			i64 index3=0;
			if(index == bitLen)
			{
				space = space + bits;
				palinspace+=bits;
				index3=index-bits;
				len=bits;
			}
			else
			{
				space = space + block_size;
				palinspace+=block_size;
				index3=index-block_size;
				len=block_size;
			}
			BitCopy1(temp,index2,data,index3,len);
		}
		else if(k==1)
		{
			if(firstbit==0)
				coding_style->SetValue((index-1)/block_size,3);
			else
				coding_style->SetValue((index-1)/block_size,4);
			space = space +0;
			all01++;
		}

		else if(codestyle==0)//rl_gamma
		{
			rlg01++;
			if(firstbit == 0)
				coding_style->SetValue((index-1)/block_size,0);
			else
				coding_style->SetValue((index-1)/block_size,1);
			space =space + rl_g;
			rlgspace+=rl_g;
			for(int i=0;i<k;i++)
			{
				Append_g(temp,index2,runs_tmp[i]);
			}
		}
		else{//EF
			ef01++;
			if(eftype==0){
				coding_style->SetValue((index-1)/block_size,5);
			}
			else{
				coding_style->SetValue((index-1)/block_size,6);
			}
			space=space+EFLen;
			if(low>0){//先存放低位数据
				for(int i=0;i<NumOfElem;i++){
					Append_f(temp,index2,lowvalue[i],low);
				}
			}
			for(int i=0;i<NumOfElem;i++){
				Append_f(temp,index2,1,gap[i]+1);
			}
		}
		if(index % step1 == 0)
		{
			pre_rank = rank;
			superblock ->SetValue(2*(index/step1),pre_rank);

			pre_space = space;
			superblock->SetValue(2*(index/step1)+1,pre_space);
		}
		if(index % step2 ==0)
		{
			block->SetValue(2*(index/step2),rank - pre_rank);
			block->SetValue(2*(index/step2)+1,space - pre_space);
		}
	}

	delete [] runs_tmp;
	int u64_len_real = 0;
	plainsize=palinspace/8;
	rlgsize=rlgspace/8;
	if(space % 64==0)
		u64_len_real = space /64+1;
	else
		u64_len_real = space /64 +1+1;
	
	this->memorysize = u64_len_real*8;
	delete [] data;
	data = new u64[u64_len_real];

	memset(data,0,u64_len_real*8);
	memcpy(data,temp,(u64_len_real-1)*8);
	delete [] temp;
}
BitMap::~BitMap()
{
	if(left)
		delete left;
	if(right)
		delete right;
	delete [] data;
	delete superblock;
	delete block;
	delete coding_style;
}
// i64 BitMap::select(i64 pos,int & bit)
// {

// }
//todo
i64 BitMap::Rank(i64 pos,int & bit)
{

	if(pos < 0 || pos > bitLen)
	{
		cerr<<pos<<"  "<<bitLen<<" BitMap::Rank(i64, int&) error parmater"<<endl;
		exit(0);
	}

	if((pos+1)%block_size!=0)//查询位置不是块中最后一位
	{
		i64 block_anchor = (pos+1)/block_size;
		i64 superblock_anchor = (pos+1)/super_block_size;
		int type =  coding_style->GetValue(pos/block_size);
		i64 rank1 = superblock->GetValue(superblock_anchor*2);
		i64 offset1 = superblock->GetValue(superblock_anchor*2+1);
		i64 rank_base = rank1+block->GetValue(block_anchor*2);
		i64 offset = offset1 + block->GetValue(block_anchor*2+1);
		buff = data +(offset>>6);
		int overloop = (pos+1)%block_size ;
		i64 index = (offset &0x3f);
		i64 rank = 0;
		i64 super_block_radio = super_block_size / block_size;
		int currentblocklen=0;
		int elemnums=0;
		if(type>4){
			i64 nextblock_anchor=block_anchor+1;
			i64 nextsuperblock_anchor=nextblock_anchor/super_block_radio;
			i64 nextrank=superblock->GetValue(nextsuperblock_anchor*2)+block->GetValue(nextblock_anchor*2);
			i64 nextoffset=superblock->GetValue(nextsuperblock_anchor*2+1)+block->GetValue(nextblock_anchor*2+1);
			currentblocklen=nextoffset-offset;
			elemnums=nextrank-rank_base;
			if(type==5){
				elemnums=block_size-elemnums;
			}
		}

		switch(type)
		{
			case 0:rank = RL_Rank(buff,index,overloop,0,bit);break;
			case 1:rank = RL_Rank(buff,index,overloop,1,bit);break;
			case 2:rank = Plain_Rank(buff,index,overloop,bit);break;
			case 3:
			{
				i64 curRankvalue = GetCurrentRankvalue(block_anchor,superblock_anchor,rank1,rank_base,super_block_radio);
				if(curRankvalue)
				{
					rank = overloop;
					bit = 1;
				}
				else
				{
					bit = 0;
					rank =0;
				}
				break;
			}
			case 4:rank = overloop;bit = 1;break;

			case 5:rank = EF_Rank(buff,index,overloop,0,elemnums,currentblocklen,bit);break;
			case 6:rank = EF_Rank(buff,index,overloop,1,elemnums,currentblocklen,bit);break;

		}

		return rank_base + rank;
	}
	else//查询位置为块中的最后一位
	{
		i64 block_anchor = (pos+1)/block_size;
		i64 superblock_anchor = (pos+1)/super_block_size;
		i64 rank1   = superblock->GetValue(superblock_anchor*2);
		i64 offset1 = superblock->GetValue(pos/super_block_size*2+1);
		i64 rank = rank1 + block->GetValue(block_anchor*2);
		i64 offset = offset1 + block->GetValue(pos/block_size*2+1);

		i64 index = (offset&0x3f);
		buff = data+(offset>>6);
		i64 type = coding_style->GetValue(pos/block_size);
		i64 super_block_radio = super_block_size / block_size;
		int currentblocklen=0;
		int elemnums=0;
		if(type>4){
			i64 prerank=superblock->GetValue(pos/block_size/super_block_radio*2)+block->GetValue(pos/block_size*2);
			i64 nextoffset=superblock->GetValue((pos+1)/block_size/super_block_radio*2+1)+block->GetValue((pos+1)/block_size*2+1);
			currentblocklen=nextoffset-offset;
			elemnums=rank-prerank;
			if(type==5){
				elemnums=block_size-elemnums;
			}
		}

		i64 overloop = block_size;
		switch(type)
		{
			case 0:bit=RL0_Bit(buff,index,overloop);break;
			case 1:bit=RL1_Bit(buff,index,overloop);break;
			case 2:bit=Plain_Bit(buff,index,overloop);break;
			case 3:
			{
				i64 curRankvalue;
				i64 rank___;
				if((pos + 1) % super_block_size == 0)
				{
					rank___ = superblock->GetValue((superblock_anchor - 1) * 2)    +  block->GetValue((block_anchor - 1) * 2);
					curRankvalue = GetCurrentRankvalue(block_anchor - 1,superblock_anchor - 1,superblock->GetValue((superblock_anchor - 1) * 2),rank___,super_block_radio);
				}
				else
				{
					rank___ = superblock->GetValue(superblock_anchor * 2)    +  block->GetValue((block_anchor - 1) * 2);
					curRankvalue = GetCurrentRankvalue(block_anchor - 1,superblock_anchor,superblock->GetValue((superblock_anchor) * 2),rank___,super_block_radio);
				}
				if(curRankvalue)
				{
					bit = 1;
				}
				else
					bit = 0;
				break;
			}
			case 4:bit=1;break;

			case 5:bit=EF0_Bit(buff,index,overloop,currentblocklen,elemnums);break;
			case 6:bit=EF1_Bit(buff,index,overloop,currentblocklen,elemnums);break;

		}

		return rank;

	}
}

void BitMap::Rank(i64 pos_left,i64 pos_right,i64 &rank_left,i64 &rank_right)
{

	if(pos_left<0 || pos_right <0 || pos_left > bitLen || pos_right > bitLen)
	{
		cout<<pos_left<<" "<<pos_right<<" "<<pos_left<<" "<<pos_right<<" "<<bitLen<<endl;
		cerr<<"BitMap::Rank(int,int,int&,int&) error parmater"<<endl;
		exit(0);
	}

	i64 block_anchor = (pos_left+1)/block_size;
	if(block_anchor==(pos_right+1)/block_size)//同一个超快内
	{
		i64 superblock_anchor=(pos_left+1)/super_block_size;
		i64 rank_base1 = superblock->GetValue(superblock_anchor<<1);
		i64 rank_base =rank_base1+ block->GetValue(block_anchor<<1);
		i64 offset1 = superblock->GetValue((superblock_anchor<<1)+1);
		i64 offset = offset1+block->GetValue((block_anchor<<1)+1);
		i64 type = coding_style->GetValue(block_anchor);
		i64 overloop_left = (pos_left+1)%block_size;
		i64 overloop_right= (pos_right+1)%block_size;
		// i64 curblocksize = GetCurrentBlocksize(block_anchor,superblock_anchor,offset1,offset);
		//hzt
		i64 super_block_radio = super_block_size / block_size;
		u64 elemnums=0;
		u64 currentblocklen=0;
		if(type>4){
			u64 next_block_anchor=block_anchor+1;
			u64 next_superblock_anchor=next_block_anchor/super_block_radio;
			u64 next_rank_base=superblock->GetValue(next_superblock_anchor<<1)+block->GetValue(next_block_anchor<<1);
			u64 nextoffset=superblock->GetValue((next_superblock_anchor<<1)+1)+block->GetValue((next_block_anchor<<1)+1);
			elemnums=next_rank_base-rank_base;
			if(type==5){
				elemnums=block_size-elemnums;
			}                                
			currentblocklen=nextoffset-offset;
		}
		//tzh
		buff = data+ (offset>>6);
		i64 index = (offset&0x3f);
		rank_left = rank_right =rank_base;
		if(overloop_left!=0)
		{
			switch(type)
			{
				case 0:RL_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right,0);break;
				case 1:RL_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right,1);break;
				case 2:Plain_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right);break;
				case 3:
				{
					i64 curRankvalue = GetCurrentRankvalue(block_anchor,superblock_anchor,rank_base1,rank_base,super_block_radio);
					if(curRankvalue)
					{
						rank_left +=overloop_left;
						rank_right += overloop_right;
					}
					break;
				}
				case 4:rank_left +=overloop_left;rank_right += overloop_right;break;
				case 5:EF_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right,0,elemnums,currentblocklen);break;
				case 6:EF_Rank(buff,index,overloop_left,overloop_right,rank_left,rank_right,1,elemnums,currentblocklen);break;
				 	i64 curblocksize = GetCurrentBlocksize(block_anchor,superblock_anchor,offset1,offset,super_block_radio);
				 	i64 curRankvalue = GetCurrentRankvalue(block_anchor,superblock_anchor,rank_base1,rank_base,super_block_radio);
					assert(curRankvalue>0);
					if (curRankvalue > (block_size >> 1))
						curRankvalue = block_size - curRankvalue;
					EF_Rank_pos(buff,index,overloop_left,rank_left,type,curblocksize,curRankvalue);
					EF_Rank_pos(buff,index,overloop_right,rank_right,type,curblocksize,curRankvalue);break;

			}
			return ;
		}
		switch(type)
		{
			case 0:rank_right += RL_Rank(buff,index,overloop_right,0);break;
			case 1:rank_right += RL_Rank(buff,index,overloop_right,1);break;
			case 2:rank_right += Plain_Rank(buff,index,overloop_right);break;
			case 3:
				{
					i64 curRankvalue = GetCurrentRankvalue(block_anchor,superblock_anchor,rank_base1,rank_base,super_block_radio);
					if(curRankvalue)
					{
						rank_right += overloop_right;
					}
					break;
				}
			case 4:rank_right += overloop_right;break;
			case 5:rank_right += EF_Rank(buff,index,overloop_right,0,elemnums,currentblocklen);break;
			case 6:rank_right += EF_Rank(buff,index,overloop_right,1,elemnums,currentblocklen);break;
			i64 curblocksize = GetCurrentBlocksize(block_anchor,superblock_anchor,offset1,offset,super_block_radio);
			i64 curRankvalue = GetCurrentRankvalue(block_anchor,superblock_anchor,rank_base1,rank_base,super_block_radio);
			assert(curRankvalue>0);
			if (curRankvalue > (block_size>>1))
				curRankvalue = block_size - curRankvalue;
			i64 rank_righttemp = 0 ;
				// EF_Rank_pos(buff, index, overloop_left, rank_left, type, curblocksize, curRankvalue);
			EF_Rank_pos(buff, index, overloop_right, rank_righttemp, type, curblocksize, curRankvalue);
			rank_right += rank_righttemp ;
			break;
			// EF_Rank_pos(buff,index,overloop_left,overloop_right,rank_left,rank_right,type-5,curblocksize);break;
		}
	}
	else
	{
		rank_left= Rank(pos_left);
		rank_right=Rank(pos_right);
	}

}
i64 BitMap::GetCurrentRankvalue_onblock(i64 block_anchor, i64 superblock_anchor, i64 offset1, i64 preoffset) //要求的点在边界上．
{
	i64 offset_1 = 0;
	if ((block_anchor ) % 16 != 0)
		offset_1 = offset1 + block->GetValue(((block_anchor - 1) << 1));
	else
		offset_1 = superblock->GetValue(((superblock_anchor-1 ) << 1)) + block->GetValue(((block_anchor - 1) << 1) );

	return preoffset-offset_1;
}
i64 BitMap::GetCurrentBlocksize_onblock(i64 block_anchor, i64 superblock_anchor, i64 offset1, i64 preoffset) //要求的点在边界上
{
	i64 offset_1 = 0;
	if (((block_anchor + 1) << 1) > block->Getdatanum() - 1)
	{
		FILEANDLINE;
	}
	else if ((block_anchor ) % 16 != 0)
	{
		auto blockv = block->GetValue(((block_anchor ) << 1) + 1);
		offset_1 = offset1 + blockv;
	}
	else
		offset_1 = superblock->GetValue(((superblock_anchor) << 1) + 1) + block->GetValue(((block_anchor) << 1) + 1);

	return offset_1 - preoffset ;
}
i64 BitMap::GetCurrentRankvalue(i64 block_anchor,i64 superblock_anchor,i64 offset1,i64 preoffset,i64 radio)//要求的点不在边界上．
{
	i64 offset_1 = 0;
	// if (((block_anchor + 1) << 1) > block->Getdatanum() - 1)
	// 	FILEANDLINE;
	if ((block_anchor + 1) % radio != 0)
		offset_1 = offset1 + block->GetValue(((block_anchor+1) << 1));
	else
		offset_1 = superblock->GetValue(((superblock_anchor+1) << 1) );

	return offset_1 - preoffset;
}
i64 BitMap::GetCurrentBlocksize(i64 block_anchor, i64 superblock_anchor, i64 offset1, i64 preoffset,i64 radio) //要求的点不在边界上
{
	i64 offset_1 = 0;
	if(((block_anchor+1)<<1) >block->Getdatanum()-1)
    {
        FILEANDLINE;
    }
	else if((block_anchor+1) % radio != 0){
		auto blockv = block->GetValue(((block_anchor + 1) << 1) + 1);
		offset_1 = offset1 + blockv;
	}
	else
		offset_1 = superblock->GetValue(((superblock_anchor+1)<<1)+1);

	return offset_1 -preoffset;
}
i64 BitMap::Rank(i64 pos)
{//bug:type value is different;
	if (pos<0 || pos > bitLen)
	{
		cerr<<"BitMap::Rank  error paramater"<<endl;
		cerr<<pos<<" "<<bitLen<<endl;
		exit(0);
	}
	// if(pos == 565031)
		// FILEANDLINE;
	i64 block_anchor = (pos+1)/block_size;//要求的所在块
	i64 superblock_anchor  = ((pos+1)/super_block_size);//要求的所在超块
	int type = coding_style->GetValue(block_anchor);

	i64 rank_base1 = superblock->GetValue(superblock_anchor<<1);
	i64 offset1 = superblock->GetValue((superblock_anchor<<1)+1);
	i64 rank_base = rank_base1 + block->GetValue(block_anchor<<1);
	i64 offset    = offset1 + block->GetValue((block_anchor<<1)+1);
	//hzt
	i64 super_block_radio = super_block_size / block_size;

	u64 elemnums=0;
	u64 currentblocklen=0;
	if(type>4){
		u64 next_block_anchor=block_anchor+1;
		u64 next_superblock_anchor=next_block_anchor/super_block_radio;
		u64 next_rank_base=superblock->GetValue(next_superblock_anchor<<1)+block->GetValue(next_block_anchor<<1);
		u64 nextoffset=superblock->GetValue((next_superblock_anchor<<1)+1)+block->GetValue((next_block_anchor<<1)+1);
		elemnums=next_rank_base-rank_base;
		if(type==5){
			elemnums=block_size-elemnums;
		}
		currentblocklen=nextoffset-offset;
	}
	//tzh
	// i64 offset_1    = 0;
	// cout<<offset_1-offset<<endl;
	// if(offset>offset_1)
	// {
	// 	cout<<__LINE__<<"wrong"<<endl;
	// }

	buff = data + (offset>>6);
	int overloop = (pos+1)%block_size ;//在块中要求的ｒａｎｋ位置
	i64 index = (offset & 0x3f);//buff 中的下标
	
	if(overloop > 0)
	{
		switch(type)//todo
		{
			case 0:rank_base += RL_Rank(buff,index,overloop,0);break;//rg0
			case 1:rank_base += RL_Rank(buff,index,overloop,1);break;//rg1
			case 2:rank_base += Plain_Rank(buff,index,overloop);break;//plain
			case 3://ALL0
			{
				i64 curRankvalue = GetCurrentRankvalue(block_anchor,superblock_anchor,rank_base1,rank_base,super_block_radio);
				if(curRankvalue)rank_base += overloop;
				break;
			}
			case 4:rank_base += overloop;break;//ALL1
			case 5:rank_base += EF_Rank(buff,index,overloop,0,elemnums,currentblocklen);break;
			case 6:rank_base += EF_Rank(buff,index,overloop,1,elemnums,currentblocklen);break;
			i64 curblocksize = GetCurrentBlocksize(block_anchor,superblock_anchor,offset1,offset,super_block_radio);
			i64 curRankvalue = GetCurrentRankvalue(block_anchor,superblock_anchor,rank_base1,rank_base,super_block_radio);
			if (curRankvalue<=0){
				cout<<"123";
				FILEANDLINE;
				}
			if (curRankvalue > (block_size>>1))
				curRankvalue = block_size - curRankvalue;
			// i64 rank = 0;
			EF_Rank_pos(buff,index,overloop,rank_base,type,curblocksize,curRankvalue);
		}
	}
	return rank_base;

}

//get designated position(index) bit value
int BitMap::GetBit(u64 * data,i64 index)
{
	int anchor = index/64;
	int pos = 63-index%64;
	return ((data[anchor] &(0x01ull<<pos))>>pos);
}


//2014.5.8:16:53:这三段程序的性能相当.
//
i64 BitMap::GetRuns(u64 * data,i64 &index,int &bit)
{
	bit = GetBit(data,index);
	index = index +1;
	int totle_runs = 1;
	int runs=0;

	while(totle_runs < block_size)
	{
		u16 x= GetBits(data,index,16);//index不联动
		if(bit==1)
			x=(~x);
		runs = Zeros(x);
		totle_runs +=runs;
		index+=runs;
		if(runs < 16)
			break;
	}
	return totle_runs;
}


//gamma编码,index联动
void BitMap::Append_g(u64 *temp,i64 &index,u32 value)
{
	u64 y=value;
	int zerosnum = blog(value)-1;
	index+=zerosnum;
	int onesnum = zerosnum+1;
	if(index%64 + onesnum < 65)//in the same longlong
	{
		temp[index/64] = (temp[index/64] | (y<<(64-(index%64 + onesnum))));
	}
	else
	{
		int first = 64 - index%64;
		int second = onesnum - first;
		temp[index/64] = (temp[index/64] | (y>>second));
		temp [index/64 +1] = (temp[index/64+1] | (y<<(64-second)));
	}
	index = index + onesnum;
}
//fix code and add to temp
//maxrl=
void BitMap::Append_f(u64 *temp,i64 &index,u64 value,u32 len)
{
	if(index%64 + len < 65)
	{
		temp[index/64] = (temp[index/64] | (value<<(64-(index%64 + len))));
	}
	else
	{
		int first = 64 - index%64;
		int second = len - first;
		temp[index/64] = (temp[index/64] | (value>>second));
		temp [index/64 +1] = (temp[index/64+1] | (value<<(64-second)));
	}
	index = index + len;
}


	void BitMap::BitCopy(u64 *temp, i64 &index, u64 value)
{
	if(index%64!=0)
	{
		int first = 64 - index % 64;
		int second = 64 - first;
		temp[index/64] = (temp[index/64] | (value>>second));
		temp[index/64 + 1] = (temp[index/64+1] | (value<<first));
	}
	else
		temp[index/64]  = value;
	index = index +64;
}

i64 BitMap::RL0_Rank(u64 * buff,i64 &index,i64 bits_num)
{
	int bit=0;
	return RL0_Rank(buff,index,bits_num,bit);
}

i64 BitMap::RL0_Rank(u64 * buff,i64 &index,i64 bits_num,int &bit)
{
	i64 rank = 0;
	i64 bit_count = 0;
	i64 bits = 0;
	while(true)
	{
		 bits=GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 if(bit_count >= bits_num)
		 {
			 bit = 0;
			 return rank;
		 }
		 bits=GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 rank = rank + bits;
		 if(bit_count >= bits_num)
		 {
			 bit = 1;
			 return rank - (bit_count-bits_num);
		 }
	}
}

i64 BitMap::RL0_Bit(u64 * buff,i64 &index,i64 bits_num)
{
	//int rank = 0;
	int bit_count =0;
	int bits = 0;
	while(true)
	{
		bits=GammaDecode(buff,index);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return  0;
		bits=GammaDecode(buff,index);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return 1;
	}
}
i64 BitMap::FRL0_Bit(u64 * buff,i64 &index,i64 bits_num)
{
	//int rank = 0;
	int bit_count =0;
	int len = 0;
	int value = 0;
	u32 x = GetBits(buff,index,32);
	int runs = Zeros(x>>16);
	int bits = (runs<<1)+1;
	index = index + bits;
	value = x>>(32-bits);
	while(true)
	{
		bits=FixedDecode(buff,index,value);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return  0;
		bits=FixedDecode(buff,index,value);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return 1;
	}
}
i64 BitMap::RL1_Rank(u64 * buff,i64 &index,i64 bits_num)
{
	int bit =0;
	return RL1_Rank(buff,index,bits_num,bit);
}

i64 BitMap::RL1_Rank(u64 * buff,i64 &index,i64 bits_num,int &bit)
{
	int rank = 0;
	int bit_count = 0 ;
	int  bits = 0;
	while(true)
	{
		 bits = GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 rank = rank + bits;
		 if(bit_count >= bits_num)
		 {
			 bit = 1;
			 return rank - (bit_count-bits_num);
		 }
		 bits = GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 if(bit_count >= bits_num)
		 {
			 bit = 0;
			 return rank;
		 }
	}
}

i64 BitMap::RL1_Bit(u64 * buff,i64 &index,i64 bits_num)
{
	int bit_count = 0 ;
	int  bits = 0;
	while(true)
	{
		 bits = GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 if(bit_count >= bits_num)
			 return 1;
		 bits = GammaDecode(buff,index);
		 bit_count = bit_count + bits;
		 if(bit_count >= bits_num)
			 return 0;
	}
}

i64 BitMap::FRL1_Bit(u64 * buff,i64 &index,i64 bits_num)
{
	int bit_count = 0;
	//int  bits = 0;
	// get block len
	int len = 0;
	int value = 0;
	u32 x = GetBits(buff,index,32);
	int runs = Zeros(x>>16);
	int bits = (runs<<1)+1;
	index = index + bits;
	value = x>>(32-bits);
	while(true)
	{
		bits=FixedDecode(buff,index,value);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return  1;
		bits=FixedDecode(buff,index,value);
		bit_count = bit_count + bits;
		if(bit_count >= bits_num)
			return 0;
	}
}
//palin类型的比例较低，所以两种方式的区别不大.
void BitMap::Plain_Rank(u64 *buff,i64 &index,i64 bits_left,i64 bits_right,i64 &rank_left,i64&rank_right)
{
	// i64 indextemp = index;
	int rank = Plain_Rank(buff,index,bits_left);

	rank_left += rank;
	index++;
	buff = buff +(index>>6);
	index = (index &0x3f);
	rank_right+= (Plain_Rank(buff,index,bits_right-bits_left)+rank);


}

i64 BitMap::Plain_Rank(u64 * buff,i64 &index,i64 bits_num,int &bit)
{
	//
#ifdef PROC
	// FILEANDLINE;
	i64 indextemp = index;
	auto i1 = GetBits(buff, indextemp, 64);
	indextemp += 64;
	auto i2 = GetBits(buff, indextemp, 64);
	indextemp += 64;
	auto i3 = GetBits(buff, indextemp, 64);
	indextemp += 64;
	auto i4 = GetBits(buff, indextemp, 64);
	// cout <<hex<< setw(20) << i1 << setw(20) << i2 << setw(20) << i3 << setw(20) << i4<<dec<<endl ;
#endif
		//
	if ((index & 0x3f) + bits_num < 65)
	{
		u64 temp = (buff[index>>6]<<(index&0x3f))>>(64-bits_num);
		index = index + bits_num -1;
		bit=(buff[index>>6]>>(63-(index&0x3f)))&0x01;
		return popcnt(temp);
	}

	int rank = 0;
	int head = 64 - (index&0x3f);
	u64 temp = (buff[index>>6]<<(index&0x3f));
	rank = rank + popcnt(temp);
	bits_num = bits_num - head;
	int times = bits_num>>6;
	int i=0;
	for(i=0;i<times;i++)
	rank = rank + popcnt(buff[i+(index>>6)+1]);
	if((bits_num&0x3f)!=0)
		rank = rank + popcnt((buff[i+(index>>6)+1] >> (64-(bits_num&0x3f))));
	index = index + head + bits_num - 1;
	bit=(buff[index>>6]>>(63-(index&0x3f)))&0x01;
	return rank;
}

i64 BitMap::Plain_Bit(u64 * buff,i64 &index,i64 bits_num)
{
#ifdef PORC
	// FILEANDLINE;
	i64 indextemp = index;
	auto i1 = GetBits(buff, indextemp, 64);
	indextemp += 64;
	auto i2 = GetBits(buff, indextemp, 64);
	indextemp += 64;
	auto i3 = GetBits(buff, indextemp, 64);
	indextemp += 64;
	auto i4 = GetBits(buff, indextemp, 64);
	 cout << i1 << "," << i2 << "," << i3 << "," << i4<<endl;
#endif
	index = index + bits_num - 1;
	return (buff[index>>6]>>(63-(index&0x3f)))&0x01;
}



i64 BitMap::Plain_Rank(u64 * buff,i64 &index,i64 bits_num)
{
	int bit=0;
	return Plain_Rank(buff,index,bits_num,bit);
}

i64 BitMap::GammaDecode(u64 * buff,i64 & index)
{
	u32 x = GetBits(buff,index,32);
	int runs = Zeros(x>>16);
	int bits = (runs<<1)+1;
	index = index + bits;
	return x>>(32-bits);
}
i64 BitMap::FixedDecode(u64 * buff,i64 &index,i64 Len)
{
	u32 x = GetBits(buff,index,32);
	//int runs = Len;
	int bits = Len;
	index = index + bits;
	return x>>(32-bits);
}

//从buff的index位置开始,读取bits位数据,返回.
u64 BitMap::GetBits(u64 * buff,i64 index,int bits)
{

	if((index & 0x3f) + bits < 65)
		return (buff[index>>6]<<(index &0x3f))>>(64-bits);

	int first = 64 - (index &0x3f);
	int second = bits - first;
	u64 high = (buff[index>>6] & ((0x01ull<<first)-1)) << second;
	return high + (buff[(index>>6)+1]>>(64-second));


}

i64 BitMap::GetZerosRuns(u64 * buff,i64 &index)
{
        u32 x = GetBits(buff,index,16);
        int runs = Zeros(x);
        index = index + runs;
        return runs;
}


void BitMap::Left(BitMap * left)
{
     this->left = left;
}


void BitMap::Right(BitMap * right)
{
     this->right = right;
}

unsigned char BitMap::Label()
{
    return label;
}


int BitMap::Load(loadkit & s)
{
	s.loadi32(level);
	s.loadu8(label);
	s.loadi64(bitLen);
	s.loadi32(block_size);
	block_width = blog(block_size);
	s.loadi32(super_block_size);
	s.loadi32(coding_tpye);
	s.loadi32(memorysize);
	this->data=NULL;
	this->superblock=NULL;
	this->block=NULL;
	this->coding_style=NULL;
	// cout<<__FILE__<<__LINE__<<":"<<s.pos()<<endl;
	if(memorysize!=0)
	{
		this->data = new u64[memorysize/8];
		s.loadu64array(data,memorysize/8);

/* liuxi*/
		superblock = new InArray();
		//cout<<"load sb:";
		superblock->load(s);
		block = new InArray();
		//cout<<"load b:";
		block->load(s);
		coding_style = new InArray();
		//cout<<"load cs:";
		coding_style->load(s);
	}
    return 0;
}


int BitMap::Save(savekit & s)
{
	s.writei32(level);
	s.writeu8(label);
	s.writei64(bitLen);
	s.writei32(block_size);
	/* liuxi*/
	s.writei32(super_block_size);
	s.writei32(coding_tpye);
	s.writei32(memorysize);
	// cout<<__FILE__<<__LINE__<<":"<<s.pos()<<endl;
	if(memorysize!=0)
	{
		s.writeu64array(data,memorysize/8);
		/* liuxi*/
		superblock->write(s);
		block->write(s);
		coding_style->write(s);
	}
	return 0;
}

i64 BitMap::RL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type)
{
	int bit=0;
	return RL_Rank(buff,index,bits_num,rl_type,bit);
}
// void BitMap::EF_Rank_pos(u64 *buff, i64 &index, int bits_pos, i64 &rank_value, int rl_type, int Blocklen, int Ranklen, int &bit)

i64 BitMap::EF_Rank_pos(u64 *buff, i64 &index, int overloop, i64 &rank_base, int type, int curblocksize, int curRankvalue)
{
	int bit = 0;
	// long long int  rank_base;
	EF_Rank_pos(buff, index, overloop, rank_base, type, curblocksize, curRankvalue, bit);
	return rank_base;
}
i64 BitMap::FRL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type)
{
	int bit=0;
	return FRL_Rank(buff,index,bits_num,rl_type,bit);
}
i64 BitMap::FRL_Rank_gap(u64 * buff,i64 & index,int bits_num,int rl_type,int Blocklen)
{
	int bit=0;
	return FRL_Rank_gap(buff,index,bits_num,rl_type,bit,Blocklen);
}

i64 BitMap::FRL_Rank_gap(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit,int Blocklen)
{
	i64 rrank =0,lrank = 0;
	FRL_Rank_gap(buff,index,0,bits_num,lrank,rrank,rl_type,Blocklen);
    return rrank-lrank;
}
i64 BitMap::FRL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit)
{
	//cout<<"FRL_Rank"<<endl;
	int offset = 0;
	int rank1  = 0;
	int value  = 0;
	int len    = 0;
	int Fixlen = 0;
	u64 x      = GetBits(buff,index,64);
	// int runs   = 64-blog64(x);
	int bits   = 3;
	offset 	  +=  bits;
	index     +=  bits;
	Fixlen     = x>>(64-bits);

	while(true)
	{
		 if(offset+Fixlen>64)
		{
		  x =  GetBits(buff,index,64);
		  offset = 0;
		}
		index  += Fixlen;
		offset += Fixlen;
		value   =  (x>>(64-offset))&((1<<(Fixlen))-1);
		if(bits_num-value<=0)
		{
			if(rl_type == 1)
				rank1 += bits_num;
			break;
		}
		else
		{
			bits_num-=value;
		}
		if(rl_type == 1)
			rank1 += value;
	//	cout<<"rl_type="<<setw(10)<<rl_type<<";value="<<setw(10)<<value<<endl;
		rl_type = 1-rl_type;

	}
	bit = rl_type;
	return rank1;
}
i64 BitMap::RL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit)
{
	// cout<<__LINE__<<"RL_Rank"<<endl;
	i64 rank=0;
	i64 r=0;
	i64 already = 0;
	u64 x = GetBits(buff,index,64);
	i64 bits = 0;//通过查找表可以解码的被编码的0,1串的长度
	i64 step = 0;//
	i64 runs =0 ;//本次解码的runs数目
	i64 runs_num = 0;//累计的runs数目
	u32 anchor=0;
	rl_type=1-rl_type;
	if(bits_num > 32)
	{
		while(true)
		{
			anchor = (x>>48)<<2;
		    runs =R[anchor];
			if(runs >0)
			{
				step =R[1+anchor];//index need to move step bits
				already = already + step;
				if(already > 64)
				{
					index = index + (already -step);
					x = GetBits(buff,index,64);
					already = 0;
					continue;
				}
				bits = R[2+anchor];//this R has bits origin code
				r=R[3+anchor];//this R has number r 1;
				bits=(bits==0)?256:bits;
				if((runs_num & 0x01) ==rl_type)
					rank = rank + r;
				else
					rank = rank + ( bits-r);
				bits_num = bits_num - bits;
				runs_num = runs_num + runs;
				if(bits_num <=0)
					break;
				x = (x<<step);
			}
			else
			{
				step = 1 + (Zeros(x>>48)<<1);
				already = already + step;
				if(already > 64)
				{
					index = index + (already - step);
					x = GetBits(buff,index,64);
					step = 1 + (Zeros(x>>48)<<1);
					already = step;
				}
				bits_num = bits_num - (x>>( 64 - step));
				if((runs_num &0x01) ==rl_type)
					rank = rank + (x>>( 64 - step));
				if(bits_num <=0)
				{
					if((runs_num &0x01)==rl_type)
					{
						bit=1;
						// cout<<rank+bits_num<<endl;
						return rank + bits_num;
					}
					else
					{
						bit=0;
						// cout<<rank<<endl;
						return rank;
					}
				}
				runs_num++;
				x = (x<<step);
			}
		}
	}
	index = index + (already - step);
	bits_num = bits_num + bits;
	runs_num = runs_num - runs;
	if((runs_num &0x01) ==rl_type)
		rank = rank - r;
	else
		rank = rank - (bits -r );
	already = 0;
	x = GetBits(buff,index,64);
	while(true)
	{
		step = 1+ (Zeros(x>>48)<<1);
		already = already + step;
		if(already > 64)
		{
			index = index + (already - step);
			x =GetBits(buff,index,64);
			step = 1+ (Zeros(x>>48)<<1);
			already = step;
		}
		bits_num = bits_num - (x>>( 64 - step));
		if((runs_num &0x01) ==rl_type)
			rank = rank + (x>>( 64 - step));
		if(bits_num <= 0)
		{
			if((runs_num&0x01)==rl_type)
			{
				bit =1;
				// cout<<rank+bits_num<<endl;
				return rank + bits_num;
			}
			else
			{
				// cout<<rank<<endl;
				bit=0;
				return rank;
			}
		}
		runs_num++;
		x=(x<<step);
	}
}

//todo
void BitMap::FRL_Rank(u64 *buff,i64 &index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type)
{
/*
	int old_index = index;
	rank_left+=RL_Rank(buff,old_index,bits_left,rl_type);
	//index = old_index;
	rank_right+=RL_Rank(buff,index,bits_right,rl_type);
*/
	bool left  = true;
	i64 offset = 0;
	i64 rank1  = 0;
	i64 value  = 0;
	i64 len    = 0;
	i64 Fixlen = 0;
	u64 x      = GetBits(buff,index,64);
	// int runs   = 64-blog64(x);
	int bits   = 3;
	offset 	  +=  bits;
	index     +=  bits;
	Fixlen     = x>>(64-bits);

	while(true)
	{
		 if(offset+Fixlen>64)
		{
		  x =  GetBits(buff,index,64);
		  offset = 0;
		}
		index  += Fixlen;
		offset += Fixlen;
		value   =  (x>>(64-offset))&((1<<(Fixlen))-1);
		bits_left = bits_left - value;//对于左，右，bits数都要减少
		bits_right = bits_right- value;
		if(rl_type == 1)
			rank1 += value;
		if(left && bits_left<=0)
		{
			if(rl_type == 1)
				{
					//rank1 += bits_left;
					rank_left += (rank1 + bits_left);
				//	rank_right += (rank1 + bits_right);
				}
				else
				{
					rank_left += rank1;
				//	rank_right += rank1;
				}
				left=false;

		}
		if(bits_right<=0)
		{
			if(rl_type == 1)
				rank_right += (rank1 + bits_right);
			else
				rank_right += rank1;
			return ;
		}
		rl_type = 1-rl_type;

	}
}

void BitMap::FRL_Rank_gapsingle(u64 *buff,i64 &index,u64 &x,i64& offset,i64 &curpos,i64 Fixlen,int bits_pos,i64 &rank_pos,i64 & rank1,int rl_type,int Blocklen)
{
	i64 value = 0;
	while(true){
        if(offset + Fixlen >64){//当前的６４比特已经读完
            x = GetBits(buff,index,64);
            offset = 0;
        }
		offset += Fixlen;
        index  +=Fixlen;
		value   =  (x>>(64-offset))&((1<<(Fixlen))-1);
        curpos +=value;
		if(	Fixlen*(rank1) + FixLenBit >= Blocklen ||  curpos>= bits_pos){
			// if(Fixlen*(rank1+1) + FixLenBit == Blocklen || curpos == bits_right)
			// if(curpos == bits_right)
				// rank1++;
            offset -= Fixlen;
            index  -=Fixlen;
            curpos -=value;
			break;
        	}
		rank1++;
	}

    if(rl_type == 1 )
        rank_pos +=(rank1 );
    else
        rank_pos += bits_pos - rank1;
	/* cout<<__LINE__<<"FRLRANK:"; */
	/* cout<<rank_left<<";"<<rank_right<<endl; */
}
void BitMap::EfLowLookup(u64 low,int lowlen,u32 selv2, u32 curvalue,int& i,bool & bhit)
{
	int movestep = 64;
	int cumRank = 0;
	const int maxContains =  16/lowlen;
	const int startindex = lowlen == 3 ? 0 : (lowlen == 4 ? 8 : 24);
	while(true){
		movestep -= maxContains*lowlen;
		u32 low16 =(low >> movestep)&0xFFFF;
		if(lowlen!=4)//move 15 but the number is count from leftmost
			low16 = low16<<1;
		auto rank = EFLow[low16*56+startindex+curvalue];
		cumRank+=rank;
		if (selv2 <= cumRank) //
		{
			i = selv2;
			break;
		}
		else if(rank != maxContains)
		{
			i = cumRank;
			if (rank != 0 && rank - EFLow[low16 * 56 + startindex + curvalue -1] ==1)//need bit info
				bhit = true;
			break;
		}

	}
}
void BitMap::EfHighLookup(u64 high, u32 bucket, u32& selv, u32& selv2)
{//if bucket == 0
	int leftmovestep = 48;
	int cumselv = 0;
	uint firstzero = 0;
	uint containOnes = 0;
	uint totalcontainOnes = 0;
	while (true)
	{
		uint byteofHigh = (high>>leftmovestep) & 0xFFFF;
		leftmovestep -=16;
		if(bucket>16){
			firstzero = EFHigh[(byteofHigh << 5 )+ 30];
			containOnes = EFHigh[(byteofHigh << 5 )+ 31];//当ｆirst < 16 时　表示接下来１的个数，否则表示最后一个合理值的位置
		}
		else if(bucket == 0){
			firstzero = 0;
			containOnes = EFHigh[(byteofHigh << 5)] - 1;
		}
		else {
			firstzero = EFHigh[(byteofHigh << 5) + ((bucket-1) << 1)];
			containOnes = EFHigh[(byteofHigh << 5) + ((bucket-1) << 1) +1];//当ｆirst < 16 时　表示接下来１的个数，否则表示最后一个合理值的位置

		}
		if(firstzero >= 16 ){
			//16 is a special index it means your need to find all imformaiton in the next 16 bits but its also competlete
			bucket -= containOnes;//here containOnes stand for indexoflastvalid
			cumselv += 16;
		}
		else if (firstzero + containOnes == 16){
			cumselv += firstzero;
			totalcontainOnes += containOnes;
			bucket = 0;
		}
		else{
			selv2 = totalcontainOnes + containOnes;
			cumselv += firstzero;
			selv = cumselv;
			break;
		}
	}
}
	//bit_pos 需要rank的位置
int flag5 = 0;
void BitMap::EF_Rank_pos(u64 *buff, i64 &index, int bits_pos, i64 &rank_value, int rl_type, int Blocklen, int Ranklen, int &bit)
{
	bits_pos--;
	const int BL = 8;
	const int I64Len  = 64;
	//
	int temp = Ranklen;
	Ranklen = Ranklen > 128 ? 256 - Ranklen : Ranklen;
	// int l = 0 > floor(log2(256 / Ranklen)) ? 0 : floor(log2(256 / Ranklen));
	int l = floor(log2(256 / Ranklen));
	const  u32 mask = ((1 << l) - 1);
	int lowlen = Ranklen * l;
	int bucketcnt = 1<<(BL-l);
	int highlen = Blocklen -lowlen;
	i64 highpos = index+lowlen;
	i64 lowpos = index;
	i64 index2 = index+Blocklen-lowlen;//
	u64 high = GetBits(buff,highpos,highlen)<<(64-highlen);
	u32 lowofbit_pos = (bits_pos) & mask;
	u32 bucket = (bits_pos >> l);
	u32 selv = 0, selv2 = 0;
	u32 zerocnt = highlen - Ranklen;
	bit = (rl_type == 5) ? 1 : 0;
	if(zerocnt >= bucket){
		#ifdef EFTABLE
		{
			flag5++;
			EfHighLookup(high, bucket, selv, selv2);
			#ifdef PROC
		cout<<dec<<setw(5)<<flag5<<",high:"<<setw(16)<<setfill('0')<<hex<<high<<setfill(' ')<<setw(10)<<dec
			<<"bucket:"<<setw(10)<<bucket<<setw(8)<<"selv1:"<<setw(10)<<selv<<setw(10)
			<<"selv2:"<<setw(10)<<selv2<<endl;
			#endif
		}
		#else
		{
		selv = 0;
		flag5++;

		if(bucket != 0){
			selv = ::selectlzero(high, bucket) + 1; //找到自己的桶 前一个桶的位置 所以第三个参数需要加1
		}

		// auto selv2 = ::firstZeroFromLeft(~high >> (highlen -selv));
		if(selv >= highlen){
			selv = 0;
		}
		selv2 = ::firstZeroFromLeft(~(high<<(selv)));//桶中元素个数
		#ifdef PROC
		// cout<<dec<<setw(5)<<flag5<<",high:"<<setw(16)<<setfill('0')<<hex<<high<<setfill(' ')<<setw(10)<<dec
		// 	<<"bucket:"<<setw(10)<<bucket<<setw(8)<<"selv1:"<<setw(10)<<selv<<setw(10)
		// 	<<"selv2:"<<setw(10)<<selv2<<endl;
			#endif
		}
		#endif
		auto onemount = selv - bucket;
		i64 i = onemount;
		lowpos += i*l;
		u64 low = GetBits(buff,lowpos,64);
		#ifdef EFTABLE
			i = 0;//　存在　l == 7　的情况
			u32 curvalue = (low>> (I64Len-l*(i+1))) & mask;
			bool  hitpos = false;
			if (selv2 >0&& (l == 3 || l == 4 || l == 5))
				EfLowLookup(low, l, selv2, lowofbit_pos, i, hitpos);
			if (hitpos)
				bit = (rl_type == 5) ? 0 : 1;
			else{
				for (i = 0; i < selv2; i++)
				{ //可替代折半查找
					u32 curvalue = (low >> (I64Len - l * (i + 1))) & mask;
					if (lowofbit_pos < curvalue)
					{
						break;
					}
					else if (lowofbit_pos == curvalue)
					{
						i++;
						bit = (rl_type == 5) ? 0 : 1;
						break;
					}
				}
			}

#else
		for(i = 0 ;i<selv2;i++){//可替代折半查找
			u32 curvalue = (low>> (I64Len-l*(i+1))) & mask;
			if(lowofbit_pos < curvalue){
				break;
			}
			else if (lowofbit_pos == curvalue){
				i++;
				bit = (rl_type == 5)? 0 : 1;
				break;
			}
		}

		#endif
			#ifdef PROC
			// cout<<setw(10)<<flag5<<setw(10)<<i<<endl;
			#endif
			if (rl_type == 6)
				rank_value += onemount + i;
			else
				rank_value += bits_pos + 1 - (onemount + i);
	}
	else
	{
		if(rl_type==6)
			rank_value += Ranklen;
		else
			rank_value += bits_pos +1- Ranklen;
	}
	// rank_value--;
	// bit = (rl_type-6)?1:0;
}
void BitMap::EF_Rank(u64 * buff,i64 &index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int ef_type,u64 elemnums,int currentBlockLen){
	u64 rank_left1=EF_Rank(buff,index,bits_left,ef_type,elemnums,currentBlockLen);
	u64 rank_right1=EF_Rank(buff,index,bits_right,ef_type,elemnums,currentBlockLen);
	rank_left+=rank_left1;
	rank_right+=rank_right1;
}
u64  BitMap::EF_Rank(u64 * buff,i64 &index,int bits_num,int ef_type,u64 elemnums,int currentBlockLen){
	int bit=0;
	return EF_Rank(buff,index,bits_num,ef_type,elemnums,currentBlockLen,bit);
}
u64  BitMap::EF_Rank(u64 * buff,i64 &index,int bits_num,int ef_type,u64 elemnums,int currentBlockLen, int &bit){
	bits_num--;
	bit=1-ef_type;
	#ifdef TESTEFRANK
	cout<<"eftype: "<<ef_type<<endl;
	#endif
	int low=blog(block_size/elemnums)-1;
	low=0>low?0:low;//计算当前块中编码时低位部分的位数
	i64 high_index=index+low*elemnums;//当前块中高位bit起始位置
	i64 low_index=index;//当前块中低位起始位置

	int highbucket=bits_num>>low;//要查询元素所在桶的编号
	int curbucket = 0;
	int prebucket = 0;
	int lowbits=bits_num&((1<<low)-1);//要查询位置的低位数值
	i64 rank=0;
	int highbucketNum = 0;
	int high_currentblocklen=currentBlockLen-low*elemnums;//当前块中高位bit数
	i64 upper_index = high_index+high_currentblocklen;

	if(highbucket==0){//当要查询的位置高位为0时，即可从头扫描当前块中的高位
		while(high_index<upper_index && GetBit(buff,high_index++)==1){//当前位置为1，则进行解码，若解码出来的位置小于要查询的位置，则rank+1
			int lowbits=GetBits(buff,low_index,low);

			low_index+=low;
			if(lowbits>bits_num){
				break;
			}
			if(lowbits==bits_num){
				bit=ef_type;
			}
			++rank;
		}

	}

	else{	//获取当前块中高位部分

		u16 i = 0;
		while(high_index < upper_index && curbucket<highbucket){
			int getbitslen = 16<upper_index-high_index ? 16:upper_index-high_index;
			i = GetBits(buff,high_index,getbitslen);
			if(getbitslen!=16)
				i = i<<(16-getbitslen);
			prebucket = curbucket;
			curbucket += R1[i];
			rank += R3[i*18]-R1[i];
			high_index += R3[i*18];
		}

		if(high_index==upper_index && curbucket<highbucket){
			return ef_type==1 ? rank:bits_num-rank+1;
		}



		else if(curbucket>=highbucket){
			rank -= R3[i*18]-R1[i];
			//high_index -= R3[i*18];
			for(int j=0;j<highbucket-prebucket;++j){
				rank += R3[i*18+j+1];
			}


			highbucketNum = R3[i*18+1+(highbucket-prebucket)];

			if(curbucket==highbucket && R3[i*18]==16 && high_index<upper_index){//一种特殊情况的处理
				//cout<<"-------special situation!----------"<<endl;
				bool flag = false;
				for(int j=highbucket-prebucket+1;j<=16;++j){
					if(R3[i*18+j+1]>0){
						flag = true;
						break;
					}
				}
				if(!flag){
					int getbitslen = 16<upper_index-high_index ? 16:upper_index-high_index;
					u16 x = GetBits(buff,high_index,getbitslen);
					if(getbitslen!=16)
						x = x<<(16-getbitslen);
					high_index += getbitslen;
					int one_runs = Zeros(~x);
					while(high_index<upper_index && one_runs==16){
						getbitslen = 16<upper_index-high_index ? 16:upper_index-high_index;
						x = GetBits(buff,high_index,getbitslen);
						if(getbitslen!=16)
							x = x<<(16-getbitslen);
						one_runs += Zeros(~x);
						high_index += getbitslen;
					}
					highbucketNum += one_runs;
				}
			}
		}

		//桶内搜索
		int decompressNum = 0;
		low_index += rank*low;
		while(decompressNum<highbucketNum){
			auto lbits = GetBits(buff,low_index,low);
			if(lbits > lowbits){
				break;
			}
			++rank;
			low_index += low;
			++decompressNum;
			if(lbits == lowbits){
				bit = ef_type;
				break;
			}
		}
	}
	return ef_type==1 ? rank:bits_num-rank+1;
}
int BitMap::EF0_Bit(u64 * buff,i64 &index,int bits,u64 currentblocklen,int elemnums)
{
	bits--;
	#ifdef TESTEFRANK
	cout<<"eftype: "<<ef_type<<endl;
	#endif
	int low=blog(block_size/elemnums)-1;
	low=0>low?0:low;//计算当前块中编码时低位部分的位数
	i64 high_index=index+low*elemnums;//当前块中高位bit起始位置
	i64 low_index=index;//当前块中低位起始位置
	int highbucket=bits>>low;//要查询元素所在桶的编号
	int curbucket = 0;
	int prebucket = 0;
	int lowbits=bits&((1<<low)-1);//要查询位置的低位数值
	i64 rank=0;
	int highbucketNum = 0;
	int high_currentblocklen=currentblocklen-low*elemnums;//当前块中高位bit数
	i64 upper_index = high_index+high_currentblocklen;

	if(highbucket==0){//当要查询的位置高位为0时，即可从头扫描当前块中的高位
		while(high_index<upper_index && GetBit(buff,high_index++)==1){//当前位置为1，则进行解码，若解码出来的位置小于要查询的位置，则rank+1
			int lowbits=GetBits(buff,low_index,low);
			low_index+=low;
			if(lowbits>bits){
				return 1;
			}
			if(lowbits==bits){
				return 0;
			}
			rank++;
		}
	}

	else{	//获取当前块中高位部分

		u16 i = 0;
		while(high_index < upper_index && curbucket<highbucket){
			int getbitslen = 16<upper_index-high_index ? 16:upper_index-high_index;
			i = GetBits(buff,high_index,getbitslen);
			if(getbitslen!=16)
				i = i<<(16-getbitslen);
			prebucket = curbucket;
			curbucket += R1[i];
			rank += R3[i*18]-R1[i];
			high_index += R3[i*18];
		}

		if(high_index==upper_index && curbucket<highbucket){
			return 1;
		}

		else if(curbucket>=highbucket){
			rank -= R3[i*18]-R1[i];
			for(int j=0;j<highbucket-prebucket;++j){
				rank += R3[i*18+j+1];
			}

			highbucketNum = R3[i*18+1+(highbucket-prebucket)];
			if(curbucket==highbucket && R3[i*18]==16 && high_index<upper_index){//一种特殊情况的处理
				bool flag = false;
				for(int j=highbucket-prebucket+1;j<=16;++j){
					if(R3[i*18+j+1]>0){
						flag = true;
						break;
					}
				}
				if(!flag){
					int getbitslen = 16<upper_index-high_index ? 16:upper_index-high_index;
					u16 x = GetBits(buff,high_index,getbitslen);
					if(getbitslen!=16)
						x = x<<(16-getbitslen);
					high_index += getbitslen;
					int one_runs = Zeros(~x);
					while(high_index<upper_index && one_runs==16){
						getbitslen = 16<upper_index-high_index ? 16:upper_index-high_index;
						x = GetBits(buff,high_index,getbitslen);
						if(getbitslen!=16)
							x = x<<(16-getbitslen);
						one_runs += Zeros(~x);
						high_index += getbitslen;
					}
					highbucketNum += one_runs;
				}
			}
		}

		//桶内搜索
		int decompressNum = 0;
		int bucketBase = highbucket<<low;
		while(decompressNum<highbucketNum){
			int decompressElem = bucketBase+GetBits(buff,index+rank*low,low);
			if(decompressElem > bits){
				break;
			}
			++rank;
			++decompressNum;
			if(decompressElem==bits){
				return 0;
			}
		}
	}
	return 1;
}
int BitMap::EF1_Bit(u64 * buff,i64 &index,int bits,u64 currentblocklen,int elemnums)
{
	bits--;
	#ifdef TESTEFRANK
	cout<<"eftype: "<<ef_type<<endl;
	#endif
	int low=blog(block_size/elemnums)-1;
	low=0>low?0:low;//计算当前块中编码时低位部分的位数
	i64 high_index=index+low*elemnums;//当前块中高位bit起始位置
	i64 low_index=index;//当前块中低位起始位置
	int highbucket=bits>>low;//要查询元素所在桶的编号
	int curbucket = 0;
	int prebucket = 0;
	int lowbits=bits&((1<<low)-1);//要查询位置的低位数值
	i64 rank=0;
	int highbucketNum = 0;
	int high_currentblocklen=currentblocklen-low*elemnums;//当前块中高位bit数
	i64 upper_index = high_index+high_currentblocklen;

	int highpos=0;
	if(highbucket==0){//当要查询的位置高位为0时，即可从头扫描当前块中的高位
		while(high_index<upper_index && GetBit(buff,high_index++)==1){//当前位置为1，则进行解码，若解码出来的位置小于要查询的位置，则rank+1
			int lowbits=GetBits(buff,low_index,low);
			low_index+=low;
			if(lowbits>bits){
				return 0;
			}
			if(lowbits==bits){
				return 1;
			}
			rank++;
			highpos++;
		}
	}

	else{	//获取当前块中高位部分

		u16 i = 0;
		while(high_index < upper_index && curbucket<highbucket){
			int getbitslen = 16<upper_index-high_index ? 16:upper_index-high_index;
			i = GetBits(buff,high_index,getbitslen);
			if(getbitslen!=16)
				i = i<<(16-getbitslen);
			prebucket = curbucket;
			curbucket += R1[i];
			rank += R3[i*18]-R1[i];
			high_index += R3[i*18];
		}

		if(high_index==upper_index && curbucket<highbucket){
			return 0;
		}

		else if(curbucket>=highbucket){
			rank -= R3[i*18]-R1[i];
			for(int j=0;j<highbucket-prebucket;++j){
				rank += R3[i*18+j+1];
			}

			highbucketNum = R3[i*18+1+(highbucket-prebucket)];
			if(curbucket==highbucket && R3[i*18]==16 && high_index<upper_index){//一种特殊情况的处理
				bool flag = false;
				for(int j=highbucket-prebucket+1;j<=16;++j){
					if(R3[i*18+j+1]>0){
						flag = true;
						break;
					}
				}
				if(!flag){
					int getbitslen = 16<upper_index-high_index ? 16:upper_index-high_index;
					u16 x = GetBits(buff,high_index,getbitslen);
					if(getbitslen!=16)
						x = x<<(16-getbitslen);
					high_index += getbitslen;
					int one_runs = Zeros(~x);
					while(high_index<upper_index && one_runs==16){
						getbitslen = 16<upper_index-high_index ? 16:upper_index-high_index;
						x = GetBits(buff,high_index,getbitslen);
						if(getbitslen!=16)
							x = x<<(16-getbitslen);
						one_runs += Zeros(~x);
						high_index += getbitslen;
					}
					highbucketNum += one_runs;
				}
			}
		}

		//桶内搜索
		int decompressNum = 0;
		int bucketBase = highbucket<<low;
		while(decompressNum<highbucketNum){
			int decompressElem = bucketBase+GetBits(buff,index+rank*low,low);
			if(decompressElem > bits){
				break;
			}
			++rank;
			++decompressNum;
			if(decompressElem==bits){
				return 1;
			}
		}
	}
	return 0;
}
void BitMap::FRL_Rank_gap(u64 *buff, i64 &index, int bits_left, int bits_right, i64 &rank_left, i64 &rank_right, int rl_type, int Blocklen)
{
    i64 offset = 0;//当前６４ｂｉｔ已经读取的位数
	i64 curpos = 0 ;//读取ｆｉｘ编码比特长度
	i64 rank1  = 0;
	i64 value  = 0;
	i64 len    = 0;
	i64 Fixlen = 0;
	u64 x      = GetBits(buff,index,64);//
	// int runs   = 64-blog64(x);
	int bits   = FixLenBit;
	offset 	  +=  bits;
	index     +=  bits;
	rl_type = 1-rl_type;//
    //curpos    += bits;
	Fixlen     = (x>>(64-bits))+1;
    FRL_Rank_gapsingle(buff,index,x,offset,curpos,Fixlen,bits_left,rank_left,rank1,rl_type,Blocklen);
    FRL_Rank_gapsingle(buff,index,x,offset,curpos,Fixlen,bits_right,rank_right,rank1,rl_type,Blocklen);
}

void BitMap::RL_Rank(u64 * buff,i64 & index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type)
{
	i64 rank = 0;//记录到解码位置的rank值
	//int rank_diff = 0;//记录left-right之间的rank值
	i64 r = 0;
	i64 already = 0;
	u64 x=GetBits(buff,index,64);
	i64 bits = 0;//通过查找表可以解码的0.1的长度
	i64 step = 0;//16位查找表可以解码的bit数
	i64 runs = 0;
	i64 runs_num = 0;
	u32 anchor = 0;
	bool left=true;//左rank还有待查找
	rl_type = 1-rl_type;

	while(true)
	{
		anchor = (x>>48)<<2;
		runs = R[anchor];
		if(runs>0)
		{
			step = R[anchor+1];
			already = already + step;
			if(already > 64)
			{
				index = index + (already - step);
				x = GetBits(buff,index,64);
				already = 0;
				continue;
			}

			bits = R[anchor+2];
			r = R[anchor+3];
			bits =(bits==0)?256:bits;
			if((runs_num & 0x01)==rl_type)
				rank  = rank + r;
			else
				rank = rank + (bits-r);

			bits_left = bits_left - bits;//对于左，右，bits数都要减少
			bits_right = bits_right- bits;
			runs_num = runs_num + runs;

			if(bits_left <= 0)
				break;
			x = (x<<step);
		}
		else
		{
			step = 1 + (Zeros(x>>48)<<1);
			already = already + step;
			if(already > 64)
			{
				index = index + (already - step);
				x = GetBits(buff,index,64);
				step = 1 + (Zeros(x>>48)<<1);
				already  = step;
			}
			bits = (x>>(64-step));
			bits_left = bits_left - bits;
			bits_right = bits_right - bits;
			r = bits;
			if((runs_num & 0x01)==rl_type)
				rank = rank + bits;
			runs = 1;
			runs_num += runs;
			if(bits_left <=0)
				break;
			x = (x<<step);
		}
	}
	index = index + (already  - step);
	bits_left = bits_left + bits;
	bits_right= bits_right+ bits;
	runs_num = runs_num - runs;
	if((runs_num & 0x01)==rl_type)
		rank = rank - r;
	else
		rank = rank - (bits -r );
	already = 0;
	x = GetBits(buff,index,64);

	while(true)
	{
		step = 1 + (Zeros(x>>48)<<1);
		already  = already + step;
		if(already > 64)
		{
			index = index + (already - step);
			x = GetBits(buff,index,64);
			step = 1 + (Zeros(x>>48)<<1);
			already = step;
		}
		bits  = (x>>(64-step));
		bits_left = bits_left - bits;
		bits_right= bits_right- bits;
		if((runs_num & 0x01)==rl_type)
			rank = rank + bits;
		if(left && bits_left <=0)
		{
			if((runs_num & 0x01)==rl_type)
				rank_left += (rank + bits_left);
			else
				rank_left += rank;
			left = false;//左rank以完成
		}

		if(bits_right <=0)
		{
			if((runs_num & 0x01)==rl_type)
				rank_right += (rank + bits_right);
			else
				rank_right += rank;
			return ;
		}
		runs_num++;
		x=(x<<step);
	}
}

//liuxi compress
void BitMap::deCompress(int &t)
{
	u32 u64Len = 0;

	if(bitLen%64 == 0)
	{
		u64Len = bitLen/64;
	}
	else
	{
		u64Len = bitLen/64+1;
	}
	assert(u64Len);

	unzipBuff = new u64[u64Len+1];
	memset(unzipBuff,0,sizeof(u64)*u64Len);

	u64 block_i=0;
	u64 deIndex=0;
	u64 zipIndex=0;

	while(deIndex < bitLen)
	{
		int errorNum =0;
		int coding_way = coding_style->GetValue(block_i++);
		switch(coding_way)
		{
			case 0:
			{
				decodeGamma(data,zipIndex , unzipBuff , deIndex ,0);
				//errorNum=testSrcAndUnzip(data, unzipBuff, deIndex, deIndex+block_size);//testSrcAndUnzip(u64 *  src ,u64 *uzip , int  start , int  end )
				if(errorNum)
				{
					cout<<" 0:RL-Gamma0"<<endl;
					cout<<" zipIndex = "<<zipIndex<<endl;
					cout<<"errorNum = "<< errorNum<<endl;
				}
				deIndex+=block_size;
			}
			break;

			case 1:
			{
				decodeGamma(data,zipIndex , unzipBuff , deIndex ,1);
				//errorNum=testSrcAndUnzip( data, unzipBuff, deIndex, deIndex+block_size);//testSrcAndUnzip(u64 *  src ,u64 *uzip , int  start , int  end )
				if(errorNum)
				{
					cout<<" 1:RL-Gamma1"<<endl;
					cout<<" zipIndex = "<<zipIndex<<endl;
					cout<<"errorNum = "<< errorNum<<endl;

				}
				deIndex+=block_size;
			}
			break;

			case 2:
			{

				decodePlain(data, zipIndex ,  unzipBuff ,  deIndex, bitLen );
				//errorNum=testSrcAndUnzip( data, unzipBuff, deIndex, deIndex+block_size );//testSrcAndUnzip(u64 *  src ,u64 *uzip , int  start , int  end )
				if(errorNum)
				{
					 cout<<" 2:Plain"<<endl;
					 cout<<" zipIndex = "<<zipIndex<<endl;
					 cout<<"errorNum = "<< errorNum<<endl;

				}
				zipIndex+=block_size;
				deIndex+=block_size;
			}
			break;

			case 3:
			{
				//errorNum=testSrcAndUnzip(data,  unzipBuff, deIndex, deIndex+block_size );//testSrcAndUnzip(u64 *  src ,u64 *uzip , int  start , int  end )
				if(errorNum)
				{
					cout<<" 3:All0"<<endl;
					cout<<" zipIndex = "<<zipIndex<<endl;
					cout<<"errorNum = "<< errorNum<<endl;

				}
				deIndex+=block_size;
			}
			break;

			case 4:
			{
				decodeAll1(unzipBuff , deIndex);
				//errorNum=testSrcAndUnzip( data,  unzipBuff, deIndex, deIndex+block_size);
				if(errorNum)
				{
					cout<<" 4:All1"<<endl;
					cout<<" zipIndex = "<<zipIndex<<endl;
					cout<<"errorNum = "<< errorNum<<endl;

				}
				deIndex+=block_size;
			}
			break;

			default:
			cout<<"some thing is wrong for deCompress!"<<endl;
			break;

		}

		if(errorNum)
		{
			exit(1);
		}

	}
	//delete[] data;
	//data = NULL;
	//t++;
	//cout << t << "Node decompression complete\n";
	return;
}
int BitMap::testSrcAndUnzip(u64 *  src ,u64 *uzip , u64  start , u64  end )
{
	if(!src || !uzip)
	{
		cout<<"测试解压是否正确接口传入参数有失误! BitMap.cpp : 566 行."<<endl;
	}
	if(end>bitLen)
	{//bian jie jian cha
		end=bitLen;
	}
	int errorNum=0;
	u64 sWords= start>>6;
	u64 eWords= end>>6;

	//cout<<eWords<<endl;
	for( u64 i=sWords;i<eWords;i++)
	{
		if(src[i] != uzip[i])
		{
			cout<<i<<"\t(";
			printBits_u64(src[i] );
			cout<<")"<<endl;
			cout<<i<<"\t_";
			printBits_u64(uzip[i] );
			cout<<"_"<<endl;
			errorNum++;
		}
		else
		{
			//cout<<i<<"\t(";
			//printBits_u64(src[i] );
			//cout<<")"<<endl;
		}
	}
	if(end&0x3f)
	{
		eWords=bitLen>>6;
		u32 x= end&0x3f;
		u32 y=64-x;
		cout<<eWords<<"\t(";
		printBits_u64(src[eWords] );
		cout<<")"<<endl;
		cout<<eWords<<"\t_";
		printBits_u64(uzip[eWords] );
		cout<<"_"<<endl;
		if((src[eWords]>>y)!=(uzip[eWords]>>y))
		{
			cout<<"warnning , bian jie cuo wu!"<<endl;
			errorNum++;
		}
	}
	return errorNum;
}
void writeG1ToUnzipbuff(u64 *unzipbuff , u64 deIndex,u64 num1)
{//there have a bug ,2016/9/20,the bug has been solved

	int words=deIndex >> 6 ;
	u64 offset =deIndex & 0x3f;
	int  tmp = num1>>6;
	u64 tmpOffset= num1 & 0x3f;
	u64 all1_64=-1;
	// if(words>5790)
	// {
	// 	cout<<words<<"\t";
	// 	printBits_u64(unzipbuff[words] );
	// 	cout<<endl;
	// }
	for(int i = 0 ; i < tmp ; i++ )
	{
		unzipbuff[words++]+=(all1_64>>offset);
		if(offset)
		{//fix bug3
			unzipbuff[words]+=(all1_64<<(64-offset));
		}
	}
	u64 hyBits=(offset+tmpOffset)&0x3f;
	if(tmpOffset)
	{
		if(((offset+tmpOffset)>>6)&&hyBits)
		{
			u64 hyBits=(offset+tmpOffset)&0x3f;
			unzipbuff[words++]+=(all1_64>>offset);
			unzipbuff[words]+=(all1_64<<(64-hyBits));
		}
		else
		{
			u64 tmpAll64=all1_64<<(64-tmpOffset);
			unzipbuff[words]+=(tmpAll64>>offset);
		}
	}
	return;
}
void BitMap::decodeAll1(u64 *unzipbuff,u64 deIndex)
{
	if(!unzipbuff)
	{
		cout<<"解压缩参数错误(BitMap.cpp, 506行)!";
	}
	int unzWords=deIndex>>6;
	u64 unzOffset = deIndex & 0x3f;
	u64 tmp=-1;
	if(bitLen-deIndex<block_size)
	{
		int num1=bitLen-deIndex;
		writeG1ToUnzipbuff(unzipbuff , deIndex,num1);
		return;
	}
	if( unzOffset )
	{//采用定长块编码,解压串不会出现解压时偏移量不等于0的情况,除非rlg解压过程中会有这种情况出现
		cout<<"解压All1时参数出错,BitMap.cpp,521 行"<<endl;
		unzipbuff[unzWords++] += ( tmp>>unzOffset ) ;
		u64 y=block_size - ( 64 - unzOffset);
		u64 yOffset = y & 0x3f;
		for(u64 i =0;i<( y >> 6);i++)
		{
			unzipbuff[unzWords++] = tmp ;
		}
		unzipbuff[unzWords++] = tmp << ( 64 - yOffset );
	}
	else
	{
		for(u64 i = 0; i < ( block_size >> 6 ); i++)
		{
			unzipbuff[unzWords++] = tmp;
		}
	}
	return;
}

void BitMap::decodePlain(u64 *zipbuff, u64 zipIndex , u64 *unzipbuff , u64 deIndex , u64 bitLen)
{
	int words = (deIndex >> 6);
	if(words > 3120)
	{
		int x=0;
		int y=0;
		int z=x+y;
	}
	int off = deIndex & 0x3f;
	if(off)
	{
		cout<<"decompress Plain error."<<endl;
	}
	if(bitLen - deIndex < block_size)
	{//bian jie chu li
		int bits= bitLen - deIndex;
		int num = bits>>6;

		for(int i = 0;i < num; i++)
		{
			u64 ttt = getValue64(zipbuff,zipIndex);
			unzipbuff[words++] = ttt;
			zipIndex += 64;
			//cout << zipIndex << endl;
		}

		int offsetBits = bits&0x3f;
		u64 tmp=getValue64( zipbuff,zipIndex);
		tmp = tmp >>(63 - offsetBits);
		unzipbuff[words] = (tmp << (63-offsetBits));

	}
	else
	{
		for(int i = 0; i< (block_size>>6) ; i++ )
		{
			unzipbuff[words++] = getValue64( zipbuff , zipIndex);
			zipIndex += 64 ;
		}
	}
	return;
}
void BitMap::decodeGamma(u64 *zipbuff,u64 &zipIndex , u64 *unzipbuff , u64 deIndex ,bool flag)
{
	int decodeBits=0;
	while(decodeBits < block_size)
	{
		if((deIndex>>6)>2264)
		{
			int x=0;
			int y=0;
			int z = x + y;
		}
		//u64 BitMap::GetBits(u64 * buff,int &index,int bits)
		i64 index=zipIndex;
		u32 x = this->GetBits( zipbuff , index , 32);//read from index (32bits)
		//u32 x =0 ;
		int runs = Zeros(x>>16);
		int bits = (runs<<1)+1;
		//index = index + bits;
		u32 num01=x>>(32-bits);
		//cout << "flag = " << flag << endl;
		if(flag)
		{
			writeG1ToUnzipbuff(unzipbuff,deIndex,num01) ;
		}

		zipIndex += bits;
		decodeBits+= num01;
		deIndex+=num01;
		flag=!flag;
	}
	return;
}
int ablog(int x)
{
	int ans = 0;
	x--;
	while (x > 0)
	{
		ans++;
		x = x >> 1;
	}
	return ans;
}
Dmap::~Dmap(void)
{
	delete[] data;
	if (rankflag)
	{
		delete Md;
		delete Sd;
		delete SBd;
		delete Bd;
	}
}

Dmap::Dmap()
{
	Md=NULL;
	Sd=NULL;
	SBd=NULL;
	Bd=NULL;
}
int fm_ablog(i64 x)
{
	int ans = 0;
	while (x > 0)
	{
		ans++;
		x = x >> 1;
	}
	return ans;
}
Dmap::Dmap(u64 len, u32 size)
{
	if (len < 0 || size <= 0)
		cout << "Dmap out" << endl;
	else
	{
		this->datanum = len;
		this->datawidth = size;
		u64 totlesize = datanum * datawidth;
		if (totlesize % 32 == 0)
			totlesize = totlesize / 32 + 1;
		else
			totlesize = (totlesize / 32) + 2;
		this->data = new u32[totlesize + 1];
		memset(data, 0, 4 * totlesize);
		//mask=((1<<datawidth)-1);
		mask = (u64)pow(2, datawidth) - 1;
	}
}
void Dmap::constructrank(int Blength,int SBlength)
{
	rankflag=1;
	this->Blength=fm_ablog(Blength)-1;//4
	this->SBlength=fm_ablog(SBlength)-1;//8
	i64 totlesize=datanum*datawidth;
	if(totlesize%32==0)
		totlesize=totlesize/32;
	else
		totlesize=(totlesize/32)+1;
	u64 sum1=0;
	for(int i=0;i<totlesize;i++)
		sum1+=(u32)__builtin_popcount(data[i]);
	u64 Mdm;
	if(datanum*datawidth%Blength==0)Mdm=datanum*datawidth/Blength+1;
	else Mdm=datanum*datawidth/Blength+2;
	Md=new InArray(Mdm,1);
	Sd=new InArray(sum1,fm_ablog(Blength));//5
	//SBd=new InArray(((datanum*datawidth/Blength+1)/SBlength+1),ablog(datanum*datawidth-Blength*SBlength+1));
    SBd=new InArray(((datanum*datawidth/Blength)/SBlength+2),fm_ablog(sum1));
	Bd=new InArray(Mdm,fm_ablog(Blength*SBlength+1));
	i64 Sdcount = 0;
	int tem;
	i64 now = -1;
	for(int i=0;i<Mdm;i++)
	{
		int flag=0;
		for(int j=0;j<Blength;j++)
		{
			now++;
			if(now>=datanum)break;
			tem=GetValue(now);
			if(tem==1)
			{
				flag=1;
				Sd->SetValue(Sdcount,j);
				Sdcount++;
			}
		}
		if(flag!=0)Md->SetValue(i,1);
		else Md->SetValue(i,0);
	}
	i64 SBnum;
	if(Mdm%SBlength==0)SBnum=Mdm/SBlength+1;
	else SBnum=Mdm/SBlength+2;
	i64 SBsum1=0;
	int flag=0;
	now=-1;
	for(i64 i=0;i<SBnum;i++)
	{
		SBd->SetValue(i,SBsum1);
		int Bdsum1=0;
		if(flag==1){
            break;
        }
		for(int j=0;j<SBlength;j++)
		{
			Bd->SetValue(i*SBlength+j,Bdsum1);
			if(flag==1){
                break;
            }
			for(int k=0;k<Blength;k++)
			{
				now++;
				if(now>=datanum){
                    flag=1;
                    break;
                }
				else{
					int tem=GetValue(now);
					if(tem==1)
					{
						SBsum1++;
						Bdsum1++;
					}
				}
			}
		}

	}
}
void Dmap::SetValue(u64 index, u64 v)
{
	if (index > datanum - 1 || index < 0)
	{
		cerr << "InArray:index out of boundary" << endl;
		exit(0);
	}
	else if (v > mask)
	{
		cerr << "InArray:value is out of boundary" << endl;
		exit(0);
	}
	else
	{
		u64 value = v;
		u64 anchor = (index * datawidth) >> 5;
		u64 temp1 = data[anchor];
		u32 temp2 = data[anchor + 1];
		temp1 = (temp1 << 32) + temp2;
		i32 overloop = ((anchor + 2) << 5) - (index + 1) * datawidth;
		if (overloop < 0)
		{
			i32 overloop2 = datawidth + overloop - 32;
			value = (value >> (-overloop));
			temp1 = temp1 + value;
			data[anchor + 1] = (temp1 & (0xffffffff));
			data[anchor] = (temp1 >> 32) & (0xffffffff);
			data[anchor + 2] = (v << (32 + overloop)) & (0xffffffff);
		}
		else
		{
			value = (value << overloop);
			temp1 = temp1 + value;
			data[anchor + 1] = (temp1 & (0xffffffff));
			data[anchor] = (temp1 >> 32) & (0xffffffff);
		}
	}
}
