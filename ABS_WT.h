/*============================================
# Filename: ABS_WT.h
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description:
  Abstract class for FM-index.the datamember root denotes
  the root bitmap of the tree.
=============================================*/
#define COUNTLFTIMES
#define __USE_FILE_OFFSET64
#define __USE_LARGEFILE64

#define _LARGEFILE64_SOURCE
#ifndef ABS_FM_H
#define ABS_FM_H
#define CHAR_SET_SIZE 256
#define CODE_MAX_LEN 256
#define PATTENLEN 20
#define _LARGEFILE64_SOURCE     /* See feature_test_macros(7) */
#include <sys/types.h>
//#define test_dmap 
#include <bitset>
#include <stdio.h>
#include <unistd.h>
#include<string.h>
// #include"BooPHF.h"
#include"BitMap.h"
#include"InArray.h"
#include"loadkit.h"
#include"savekit.h"
#include"./divsufsort/divsufsort64.h"
#include <map>
#include<vector>
#include <future>
#include <mutex>
#include <pthread.h>
#include <omp.h>
#include "sampler.h"
#include"BaseClass.h"
using namespace std;
using namespace FMM;
uint32_t select(uint64_t x, uint32_t i);
uint32_t selectlzero(uint64_t x, uint32_t i);
u32 firstZeroFromLeft(u64 n);
int popcnt(unsigned long long int x);
class ABS_FM
{
	typedef long long fm_int;
	public:
		//D:SA数组采样步长,Rank采样步长D*16
		u64 bwtIndex;
		ABS_FM(const char *filename, int block_size = 256, int D = 16);
		ABS_FM() { enterposflag = false; };
		virtual ~ABS_FM();
		void Counting(const char *partten, i64 &num);
		i64 *Locating_value(const char *pattern, i64 &num);
		i64 *Locating_pos(const char *pattern, i64 &num);
		i64 *Locating2(const char *pattern, i64 &num);
		i64 *Locating_parrel(const char *pattern, i64 &num);
		unsigned char *Extracting(i64 pos, i64 len);
		unsigned char *Extracting_parrel(i64 pos, i64 len);
		int Load(loadkit &s);
		int LoadSAL(loadkit &s);
		int Save(savekit &s);
		int SaveSAL(savekit &s, char savemode);
		int BuildTree(int speedlevel = 1, int method = SampleMethod::ordinary, bool calsample = false);
		int decompress_all(const char *filename);
		virtual int buildIndex(int SampleMethod =0);
		void DrawBackSearch(const char *pattern, i64 &Left, i64 &Right);
		void testorigin(Sampler *sampler, saidx64_t *SA, bool originweightmethod,uint* weight);
		void testmy(Sampler *sampler, saidx64_t *SA, bool originweightmethod,uint* weight);
		vector<i64> getencoderadio();
		i64 getrlgradio(BitMap *r);
		i64 getefradio(BitMap *r);
		i64 getplainradio(BitMap *r);
		i64 getallradio(BitMap *r);
		int GetAlphabetsize() { return alphabetsize; }
		i64 GetN() { return n; }
		i64 SizeInByte();
		i64 SizeInByte_count();
		i64 SizeInByte_extract();
		i64 SizeInByte_locate();
		i64 SizeInText(){return n;}
		void Codedistribution(int &Plain, int &AL0, int &AL1);
		BitMap *GetRoot();
		unsigned char *bwt;
		saidx64_t *sa;
		void SASample(saidx64_t* SA);
		void SASample_enter(saidx64_t *SA);
		void SASample_hash(saidx64_t *SA);
		void DisSample(string disFilename);
		vector<string> AllPossiblePattern(string str, int len);
		Sampler *CalSampler(int lengthOfPattern, saidx64_t *SA,bool orginweightmethod);
		void DisSample(Sampler *sampler, saidx64_t *sa,bool orginweightmethod);
		//test
		i64 SizeOfpart(BitMap *r, string str);
		map<i64, i64> BWTruns;
		map<i64, i64> MergeBitMapRuns(BitMap *r);
		map<i64, i64> getBitMapRuns();
		map<i64, i64> getBitnodeRuns();
		void getbwtRuns(unsigned char *bwt, int len);
		i64 SizeInBytePart_count(string str);
		map<i64, i64> MergeBitnodeRuns(BitMap *r);
		//map<i64, i64> MergeBitMapRuns(BitMap *r);
		i64 sizeOfSAL();
		i64 sizeOfRankL();
		i64 Lookup(i64 i);
		i64 Lookupfornew(i64 i);
		void Getpos(fm_int left,fm_int right,fm_int *&pos);
		bool enterposflag;
		Dmap *GetEnterPos()
		{
			if (enterposflag == false){
				i64 num = 0;
				if(newD==0)
				{
					penterposvec = Locating_pos("\n", num);
					cout<<"newD=0!"<<endl;
				}
				else
				{
					penterposvec = Locating_value("\n", num);
				}
				sort(penterposvec, penterposvec + num);
				int width = log2(n);
				newInEnter = new Dmap(n, 1);
				for (i64 i = 0; i < n; i++)
				{
					newInEnter->SetValue(i,0);
				}
				for (i64 i = 0; i < num; i++)
				{
					newInEnter->SetValue(penterposvec[i],1);
				}
				newInEnter->constructrank(256, 16);
				enterposflag = true;
			}
			return newInEnter;
		};
		i64 getLFtimes(){return LFtimes;};
		i64 GetEnterPosRank(i64 i);
		i64 getLocatetimes() { return Locatetimes; };
		i64 Lookupforhash(i64 i);
		void buildEFtable(u64 i);
		double gettotalruns(){return total_runs;}
		// boophf_t *bphf; //hash
#ifdef COUNTLFTIMES
			unsigned long long LFtimes;
		unsigned long long Locatetimes;
#endif
	protected:
		BitMap * root;
		BitMap *posenter;
		Dmap   *newD;
		uchar * Z;
		uchar * R;
		uchar * R1;
		uchar * R3;
		uchar * EFHigh;
		uchar * EFLow;
		void Inittable();
		unsigned char *T;

		int block_size; //the block_size of wavelettree's node.default 1024
		int D;		 //step of SAL
		InArray *InEnter;   //the sampling of SA array
		InArray *SAL;   //the sampling of SA array
		InArray *RankL; // The sampling of the Rank Array
		InArray *SALPos;//Sample booleam;
		Dmap *newInEnter;
		bool charMap[256];
		// if charMap[i] is true,char[i] is a member of alphabet

		i64 *C; // Cumulative Frequency sum
		i16 *code;
		i64 charFreq[CHAR_SET_SIZE];
		i32 alphabetsize;
		i32 alphabetsize2;
		char codeTable[CHAR_SET_SIZE][CODE_MAX_LEN];
		i64 Occ(i64 &rank, unsigned char &label, i64 pos);
		i64 Occ(unsigned char c, i64 pos);
		void Occ(unsigned char c, i64 pos_left, i64 pos_right, i64 &rank_left, i64 &rank_right);
		i64 LF(i64 i);
		unsigned char L(i64 i);
		i64 LookupALL(i64 startpos);
		void Lookup(i64 startpos,i64 endpos,vector<i64>& v_i64);
		virtual int TreeCode() { return -1; };
		int writeToFile(const char *filename, unsigned char *T);
		u64 binarysearchLower(const int *A, u64 size, u64 value);
		int inv_bw_transform(unsigned char *bwt, unsigned char *T, u64 n, u64 idx);
		int inv_sa_transform(unsigned char *bwt, saidx64_t *sa, u64 n, u64 idx,int method);
		int id_inv_sa_transform(unsigned char *bwt, saidx64_t *sa, u64 n, u64 idx,int method);
		int BWT(unsigned char *T, int *SA, unsigned char *bwt, int n);
		int BWT64(unsigned char *T, saidx64_t *SA, unsigned char *bwt, saidx64_t len);
		BitMap *CreateWaveletTree(unsigned char *bwt, i64 n,char ctable[CHAR_SET_SIZE][CHAR_SET_SIZE],int method);
		BitMap * FullFillWTNode(unsigned char * buff,i64 len,int level,char ctable[CHAR_SET_SIZE][CHAR_SET_SIZE],int method);//ct = codetable
		int DestroyWaveletTree();
		int blog(int);
		unsigned char *Getfile(const char *filename);
		//unsigned char *Getfile(const char *filenam,int part,int pos);
		int SaveNodePosition(BitMap *, u32, savekit &);
		int SaveNodeData(BitMap *, savekit &s);
		int SaveWTTree(savekit &s,BitMap* r);
		BitMap* LoadWTTree(loadkit &s,int alphabetsize,uchar **tables = NULL);

		int TreeNodeCount(BitMap *root);

		i64 TreeSizeInByte(BitMap *r);
		//	int TreeSizeInByte();
		friend int GammaDecode(u64 *buff, int &index, ABS_FM *t);
		friend int Zeros(u16 x, ABS_FM *t);
		void Test_L();
		void Test_Occ();
		void Test_Shape(BitMap *);
		//decompress-liu
		int genBwtWithWaveletTree(unsigned char *bwt);
		u64 getBWTIndex() { return bwtIndex; }
		int decmpWaveletTree(BitMap *root, int &t);
		int decompressInitWaveletTree(BitMap *root);
		
		i64 *penterposvec;
		i64 num;
		bool oldsample;

	  private:
		const char *filename;
		i64 n;
		double total_runs;
};

#endif

