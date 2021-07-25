/*============================================
# Filename: BitMap.h
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description: Hybrid-bitmap,support rank(int pos) and rank(int pos,int &bit)
=============================================*/
// #define PROC 0
// #define OUTPUTCODE
// #define EFTABLE
#ifndef WT_NODE_H
#define WT_NODE_H
#include<string.h>
#include"loadkit.h"
#include"savekit.h"
#include"InArray.h"
#include"BaseClass.h"
#include<math.h>
#include<iostream>
#include <map>
#include <vector>
#include<assert.h>
#include"select.h"
//#define ef_coding
using namespace std;
#define FILEANDLINE cout<<__FILE__<<":"<<__LINE__<<setw(20);
extern  uint32_t select(uint64_t x, uint32_t i);
extern  uint32_t selectlzero(uint64_t x, uint32_t i);
extern  u32 firstZeroFromLeft(u64 n);
extern  int popcnt(unsigned long long int x);
class Dmap
{
	typedef long long fm_int;
  public:
	Dmap();
	u64 GetMemorySize()
	{
		if(Md==NULL)
			return 0;
		else
			return Md->GetMemorySize() + Sd->GetMemorySize() + SBd->GetMemorySize() + Bd->GetMemorySize();
		};
	Dmap(u64 data_num, u32 data_width);
	i64 write(savekit &s){
		// u32 *data;
		s.writeu64(datanum);
		s.writeu32(datawidth);
		s.writeu64(mask);
		s.writei32(rankflag);
		s.writei32(Blength);
		s.writei32(SBlength);
		Md->write(s);
		Sd->write(s);
		SBd->write(s);
		Bd->write(s);
	};
	i64 load(loadkit &s){
		s.loadu64(this->datanum);
		s.loadu32(this->datawidth);
		s.loadu64(this->mask);
		s.loadi32(this->rankflag);
		s.loadi32(this->Blength);
		s.loadi32(this->SBlength);
		this->Md  = new InArray();
		this->Sd  = new InArray();
		this->SBd = new InArray();
		this->Bd  = new InArray();
		this->Md->load(s);
		this->Sd->load(s);
		this->SBd->load(s);
		this->Bd->load(s);
	};
	~Dmap(void);
	void SetValue(u64 index, u64 v);
	void constructrank(int Blength, int SBlength);
	u64 GetValue(u64 index)
	{
		if (index > datanum - 1 || index < 0)
		{
			cerr << "InArray:GetValue: index out of boundary" << endl;
			exit(0);
		}
		u64 anchor = (index * datawidth) >> 5;
		u64 temp1 = data[anchor];
		u32 temp2 = data[anchor + 1];
		u32 temp3 = data[anchor + 2];
		i32 overloop = ((anchor + 2) << 5) - (index + 1) * datawidth;
		temp1 = (temp1 << 32) + temp2;
		if (overloop < 0)
		{
			temp1 = (temp1 << (-overloop)) + (temp3 >> (32 + overloop));
			return temp1 & mask;
		}
		else
			return (temp1 >> overloop) & mask;
	}
	int getD(u64 i)
	{
		u64 Mdnum = i >> Blength;
		if (Md->GetValue(Mdnum) == 0)
			return 0;
		else
		{
			u64 SBnum = Mdnum >> SBlength;
			u64 Sbdbefore = SBd->GetValue(SBnum);
			u64 Sdnowi = Sbdbefore + Bd->GetValue(Mdnum);
			u64 Sdnow = Sd->GetValue(Sdnowi);
			if (((Mdnum << Blength) + Sdnow) > i)
				return 0;
			else if (((Mdnum << Blength) + Sdnow) == i)
				return 1;
			else
			{
				u64 temnum1 = 0;
				if (Mdnum != Md->GetNum() - 1)
				{

					u64 nextBd = Bd->GetValue(Mdnum + 1);
					if (nextBd == 0)
						temnum1 = SBd->GetValue(SBnum + 1) - Sbdbefore - Bd->GetValue(Mdnum);
					else
						temnum1 = nextBd - Bd->GetValue(Mdnum);
				}
				else
				{
					temnum1 = Sd->GetNum() - Sdnowi;
				}
				for (u64 j = 1; j < temnum1; j++)
				{
					Sdnow = Sd->GetValue(Sdnowi + j);
					if (((Mdnum << Blength) + Sdnow) > i)
						return 0;
					else if (((Mdnum << Blength) + Sdnow) == i)
						return 1;
				}
				return 0;
			}
		}
	}
	u64 rank(u64 i)
	{
		u64 Mdnum = i >> Blength;
		u64 SBnum = Mdnum >> SBlength;
		u64 Sbdbefore = SBd->GetValue(SBnum);
		u64 Sdnowi = Sbdbefore + Bd->GetValue(Mdnum);
		while (1)
		{
			u64 Sdnow = Sd->GetValue(Sdnowi);
			if (((Mdnum << Blength) + Sdnow) == i)
				break;
			Sdnowi++;
		}
		return Sdnowi + 1;
	}
	u64 rank2(u64 i)
	{
		u64 Mdnum = i >> Blength;
		u64 low = i & ((1 << Blength) - 1);
		u64 SBnum = Mdnum >> SBlength;
		u64 Sbdbefore = SBd->GetValue(SBnum);
		u64 Sbdafter = SBd->GetValue(SBnum+1);
		u64 Sdnowi = Sbdbefore + Bd->GetValue(Mdnum);
		if(Md->GetValue(Mdnum) == 0)
			return Sdnowi;
		if((Mdnum+1)%16 == 0)
		{
			while(1)
			{
				u64 Sdnow = Sd->GetValue(Sdnowi);
				if(low == Sdnow)
				{
					Sdnowi++;
					break;
				}
				else if(low > Sdnow)
				{
					Sdnowi++;
					if(Sdnowi >= Sbdafter)
						break;
				}
				else
					break;
			}
		}
		else
		{
			u64 Sdagap = Bd->GetValue(Mdnum+1)-Bd->GetValue(Mdnum);
			i64 k = 0;
			while(1)
			{
				u64 Sdnow = Sd->GetValue(Sdnowi);
				if(low == Sdnow)
				{
					Sdnowi++;
					break;
				}
				else if(low > Sdnow)
				{
					Sdnowi++;
					k++;
					if(k >= Sdagap)
						break;
				}
				else
					break;
			}
		}
		return Sdnowi;
	}
	i64 select(i64 i)
	{
		if(i<=0||i>Sd->GetNum()){
			cout<<"select out of boundary"<<endl;
			return 0;
		}
		int b=1<<Blength;
		int sb=1<<SBlength;
		fm_int Mdnum;
		if(datanum%b==0)Mdnum=datanum/b+1;
		else Mdnum=datanum/b+2;
		fm_int SBnum;
		if(datanum%(sb*b)==0)SBnum=datanum/(sb*b)+1;
		else SBnum=datanum/(sb*b)+2;
		fm_int Sbpos=BS(SBd,0,SBnum-1,i);
		// cout<<"Sbpos: "<<Sbpos<<endl;
		fm_int l=Sbpos*sb;
		fm_int r=(Sbpos+1)*sb-1;
		if(r>=Mdnum)
			r=Mdnum-1;
		fm_int k=i-SBd->GetValue(Sbpos);
		fm_int Bpos=BS(Bd,l,r,k);
		// cout<<"bpos: "<<Bpos<<endl;
		fm_int finalpos=Bpos*b+Sd->GetValue(i-1);
		return finalpos;
	}
i64 BS(InArray *B,i64 l,i64 r,i64 i)//二分查询第i个1所在的块
{
	if(B->GetValue(l)==i) return l-1;
	if(B->GetValue(r)==i) {
		while(B->GetValue(r)==i){
			r--;
		}
		return r;
	}
	if(B->GetValue(r)<i) return r;
	while(l<=r){
		auto midpos=l+((r-l)>>1);
		// cout<<l<<" ## "<<midpos<<" ## "<<r<<endl;
		if(midpos==l){
			if(B->GetValue(l)>=i){
				return l-1;
			}
			else{
				return l;
			}
		}
		auto mid=B->GetValue(midpos);
		if(mid<i) l=midpos+1;
		else if(mid>=i) r=midpos;
	}
}
  private:
	u32 *data;
	u64 datanum;
	u32 datawidth;
	u64 mask;
	int rankflag;
	InArray *Md;
	InArray *Sd;
	InArray *SBd;
	InArray *Bd;
	int Blength;
	int SBlength;
};
class BitMap
{
	public:
		//static i64 Gamacount ;

		BitMap(unsigned long long int * bitbuff,i64 bit_len,int level,int method,int block_size=1024,unsigned char label='\0',uchar ** tables=NULL);
		//bit_len:0,1串的实际长度，单位bit
		//level:层数
		//block_size:块大小
		//label:当前节点代表的字符.只有叶节点的label域又意义.

		BitMap(){};
		BitMap(uchar ** tables):Z(tables[0]),R(tables[1]),R1(tables[2]),R3(tables[3]){}
		~BitMap();

		i64 Rank(i64 pos);
		i64 Rank(i64 pos,int &bit);
		void Rank(i64 pos_left,i64 pos_right,i64 &rank_left,i64 &rank_right);


		void Left(BitMap * left);
		//设置左孩子

		BitMap * Left(){return left;};
		//返回左孩子

		void Right(BitMap * right);
		//...

		BitMap * Right(){return right;};
		//...
		//test
		static map<i64,i64> mapruns;
		static map<i64,i64> noteruns;
		static i64 ALLcount;
		static i64 Othercount;
		static i64 RLcount;
		static i64 EFcount;
		static i64 Plaincount;
		static i64 plainSize;
		static i64 FixSize;
		static i64 EFSize;
		static i64 RLSize;
		static i32 Block_size;
		static i32 superblock_size;
		//test
		unsigned char Label();
		int Load(loadkit & s);
		int Save(savekit & S);
		i64 SizeInByte();
		i64 SizeInSuperblock();
		i64 SizeIncodesytle();
		i64 SizeInblock();
		i64 SizeInMemory();

		//---decompress--
		int testSrcAndUnzip(u64 *src, u64 *uzip, u64 start, u64 end);
		void decodeAll1(u64 *unzipbuff, u64 deIndex);
		void decodePlain(u64 *zipbuff, u64 zipIndex, u64 *unzipbuff, u64 deIndex, u64 bitLen);
		void decodeGamma(u64 *zipbuff, u64 &zipIndex, u64 *unzipbuff, u64 deIndex, bool flag);
		void deCompress(int &t);
		int getcodingtype(){return coding_tpye;}
		i64 getrlgradio(){return rlg01;}
		i64 getplainradio(){return plain;}
		i64 getefradio(){return ef01;}
		i64 getallradio(){return all01;}
		//decompree-liu
		u64 *unzipBuff;
		u64 unzipLen;
		u64 *dePtr;
		int deOff;

	  private:
		uchar* Z;
		//uchar* R1;
		//uchar* R2;
		//uchar* R3;
		//uchar* R4;
		uchar *R;
		uchar *R1;
		uchar *R3;
		uchar *EFHigh;
		uchar *EFLow;
		BitMap(const BitMap &);
		BitMap & operator =(const BitMap& right);
		void Coding();
		void Coding2();
		void EfHighLookup(u64 high, u32 bucket, u32 &selv, u32 &selv2);
		void EfLowLookup(u64 low, int lowlen, u32 selv2, u32 curvalue, int &i, bool &bhit);

		//得到存储在data中的0,1串中的第index位
		int GetBit(u64 *data, i64 index);
		//从buff保存的0.1串中，由index位置开始，返回后续bits位表示的
		//数值.

		u16 Zeros(u16 x){return (Z[x>>8]==8)?Z[x>>8]+Z[(uchar)x]:Z[x>>8];}
		u64 GetBits(u64 * buff,i64 index,int bits);
		void BitCopy1(u64 * temp,i64 &index,u64*data,i64 index1,int len);
		//得到0的runs.
		i64 GetZerosRuns(u64 * buff,i64 &index);
		//gamma解


		i64 FixedDecode(u64 * buff,i64 &index,i64 Len);
		i64 GammaDecode(u64 * buff,i64 &index);
		vector<int> ConvertRLtopos(int *runs,int num,int firstbit,int& bit);
		int* ConvertRLtogap(vector<int> pos,int low);
		//得到0,1串中的runs长度，bit标示该runs是针对谁的
		i64 GetRuns(u64 * data,i64 &index,int &bit);
		//index64
		void Append_g(u64 * temp,i64 &index,u32 value);
		void Append_f(u64 * temp,i64 &index,u64 value,u32 maxrl);
		//把u64类型的value拷贝到data串的index处.
		void BitCopy(u64 * temp,i64 &index,u64 value);

		//返回rl0编码的串中，由index位置开始，长度位bits
		//内的1的个数.
		void RL_Rank(u64 * buff,i64 & index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type);
		i64  RL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type);
		i64  RL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit);
		i64 FRL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit);
		i64 FRL_Rank(u64 * buff,i64 & index,int bits_num,int rl_type);
        void FRL_Rank(u64 *buff,i64 & index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type);
		i64 FRL_Rank_gap(u64 * buff,i64 & index,int bits_num,int rl_type,int &bit,int BlockLen);
		i64 FRL_Rank_gap(u64 * buff,i64 & index,int bits_num,int rl_type,int BlockLen);
        void FRL_Rank_gap(u64 *buff,i64 & index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int rl_type,int BlockLen);
		void EF_Rank_pos(u64 *buff, i64 &index, int bits_pos, i64 &rank_value, int rl_type, int Blocklen, int Ranklen,int& bit);
		// i64 EF_Rank_pos(buff, index, overloop, rank_base, type, curblocksize, curRankvalue);
		i64 EF_Rank_pos(u64 *buff, i64 &index, int overloop, i64 &rank_base, int type, int curblocksize, int curRankvalue); //返回容量编码的串中，由index位置开始，bits位内的0的个数.
		int findzero(u64 *bits,int len,int k);
		i64 RL0_Rank(u64 *buff, i64 &index, i64 bits_num);
		i64 RL0_Rank(u64 * buff,i64 & index,i64 bits_num,int &bit);
		//返回容量编码的串中，由index位置开始，bits位内的1的个数.
		i64 RL1_Rank(u64 * buff,i64 & index,i64 bits);
		i64 RL1_Rank(u64 * buff,i64 & index,i64 bits,int &bit);
		//返回容量编码的串中，由index位置开始，bits位内的1的个数.
		i64 RL1_Bit(u64 * buff,i64 & index,i64 bits_num);
		i64 RL0_Bit(u64 * buff,i64 & index,i64 bits_num);
		i64 FRL1_Bit(u64 * buff,i64 & index,i64 bits_num);
		i64 FRL0_Bit(u64 * buff,i64 & index,i64 bits_num);
		void EF_Rank(u64 * buff,i64 &index,int bits_left,int bits_right,i64 &rank_left,i64 &rank_right,int ef_type,u64 elemnums,int currentBlockLen);
		u64  EF_Rank(u64 * buff,i64 &index,int bits_num,int ef_type,u64 elemnums,int currentBlockLen);
		u64  EF_Rank(u64 * buff,i64 &index,int bits_num,int ef_type,u64 elemnums,int currentBlockLen,int &bit);
		int EF0_Bit(u64 * buff,i64 &index,int bits,u64 currentblocklen,int elemnums);
		//返回容量编码的串中，由index位置开始，bits位内的1的个数.
		int EF1_Bit(u64 * buff,i64 & index,int bits,u64 currentblocklen,int elemnums);
		//int RL1_Bit(u64 * buff,int & index,int bits);
		//int RL1_Rank(u64 * buff,int &index,int bits,int &bit);
		//buff从index位置开始是直接存储的，从index位置开始，bits
		//位内有几个1.
		void Plain_Rank(u64 *buff,i64 &index,i64 bits_left,i64 bits_right,i64 &rank_left,i64&rank_right);
		i64 Plain_Rank(u64 * buff,i64 &index,i64 bits);
		i64 Plain_Bit(u64 * buff,i64 &index,i64 bits);
		i64 Plain_Rank(u64 * buff,i64 &index,i64 bits,int & bit);
		vector<int> ConvertGapTODiv(vector<int> gaps,i64& firstbit);
		vector<int> ConvertRLtoGap(int * run,int len,i64& firstbit);
		vector<int> ConvertRLtoDiv(int * run,int len,i64& firstbit);
		int level;//该串的层数.
		i64 GetCurrentBlocksize(i64 block_anchor,i64 superblock_anchor,i64 offset1,i64 preoffset,i64 radio);
		i64 GetCurrentRankvalue(i64 block_anchor,i64 superblock_anchor,i64 offset1,i64 preoffset,i64 radio);
		i64 GetCurrentRankvalue_onblock(i64 block_anchor, i64 superblock_anchor, i64 offset1, i64 preoffset);
		i64 GetCurrentBlocksize_onblock(i64 block_anchor, i64 superblock_anchor, i64 offset1, i64 preoffset);
		unsigned char label;
		//只有叶节点又意义，表示该节点代表的字符

		void FRL_Rank_gapsingle(u64 *buff,i64 &index,u64 &x,i64& offset,i64 &curpos,i64 Fixlen,int bits_pos,i64 &rank,i64 & rank1,int rl_type,int Blocklen);
		// void FRL_Rank_gapsingle(u64 *buff,i64 &index,i64 offset,i64 Fixlen,i64 &curpos,i64 x,int bits_pos,i64 &rank_pos,i64& rank1,int rl_type,int Blocklen);
		unsigned long long int * data;		//0,1串的压缩存储体.

		i64 bitLen;		//0,1串的长度，单位bit。

		i32 memorysize;
		int block_size;
		int block_width;
		int super_block_size;
		int coding_tpye;//gamma/efcode
		BitMap * left;
		BitMap * right;
		int rate = 16;
		InArray *superblock;//超快偏移量
		InArray *block;//块的偏移量
//		InArray *superblock_rank;//超快的偏移量
//		InArray *block_rank;//块的偏移量
		InArray *coding_style;//每个块的编码方案.0:plain, 1:RLG0, 2:RLG1;
		//wch add for fixed FixedDecode
		InArray *nbit;
		InArray *Flag;
		u64 all01=0;
		u64 plain=0;
		u64 rlg01=0;
		u64 ef01=0;
		u64 plainsize=0;
		u64 rlgsize=0;
		u64 efsize=0;
		//wch add
		//这是个工作变量.
		unsigned long long int * buff;
};

#endif







