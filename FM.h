/*
 * @Author: your name
 * @Date: 2021-05-30 15:33:07
 * @LastEditTime: 2021-07-18 17:23:39
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CIndex-master/FM.h
 */
/*============================================
# Filename: WT_Handle.h
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description:
  A handle class for pointer fm(ABS_FM *),ABS_FM will
  actual points to a specific subclass.

=============================================*/
#ifndef WTHANDLE_H
#define WTHANDLE_H
#include"UseCount.h"
#include"ABS_WT.h"
// extern  Method amethod;
class FM
{
  public:
	ABS_FM *fm;
	UseCount u;
	int mtd;
  public:
	FM();
	// FM(globalArgs_t globalArgs):FM(globalArgs.FILENAME.c_str(), globalArgs.BLOCKSIZE, globalArgs.SAMPLERATE, 1, 1, globalArgs.SAMPLEMETHOD, globalArgs.calsample); };
	FM(const char *filename, int block_size = 256, int D = 32,  int shape = 1, int speedlevel = 1,int method = SampleMethod::ordinary, bool calsample = false);
	FM(const FM &);
	FM &operator=(const FM &);
	~FM();
	void DrawBackSearch(const char *pattern, i64 &Left, i64 &Right) { fm->DrawBackSearch(pattern, Left, Right); }
	void Counting(const char *pattern, i64 &num) { fm->Counting(pattern, num); };
	i64 Lookup(i64 startpos) { return fm->Lookup(startpos); };
	// vector<i64> GetHittimes() { return fm->GetHittimes(); };
	i64 *Locating(const char *pattern, i64 &num) {
//		if (mtd == SampleMethod::ordinary)
//			return fm->Locating_pos(pattern, num);
//		else
			return fm->Locating_value(pattern, num);
			 };
	i64 *Locating_parrel(const char *pattern, i64 &num) { return fm->Locating_parrel(pattern, num); };
	void Codedistribution(int &Plain,int &AL,int &other){fm->Codedistribution(Plain,AL,other);};
	//void  GetMaps(Map<i64,i64> &bwtmap,Map<i64,i64> &runsmap){fm->GetMap(bwtmap,runsmap)};
	unsigned char *Extracting(i64 pos, i64 len) { return fm->Extracting(pos, len); };
	unsigned char *Extracting_parrel(i64 pos, i64 len) { return fm->Extracting_parrel(pos, len); };
	int Load(const char *indexfile);
	int LoadSAL(const char *indexfile);
	int Save(const char *indexfile);
	int Save_qv(const char *indexfile);
	int SaveSAL(const char *indexfile,char savemode);
	double compressRatio();
	i64 GetN() { return fm->GetN(); }
	int GetAlphabetsize() { return fm->GetAlphabetsize(); }
	i64 SizeInByte() { return fm->SizeInByte(); };
	i64 SizeInByte_count() { return fm->SizeInByte_count(); };
	i64 SizeInByte_extract() { return fm->SizeInByte_extract(); };
	i64 SizeInByte_locate() { return fm->SizeInByte_locate(); };
	i64 SizeInText() { return fm->SizeInText(); }
	int Decompres_FILE(const char *filename) { return fm->decompress_all(filename); };
	int BuildIndex(int SampleMethod) { return fm->buildIndex(SampleMethod); };
	// vector<i64>& GetEnterPos() { return fm->GetEnterPos(); };
	// void GetEnterPos(vector<i64> & vec) { fm->GetEnterPos(vec); };
	Dmap* GetEnterPos() { return fm->GetEnterPos(); };
	i64 GetEnterPosRank(i64 i) {return  fm->GetEnterPosRank(i ); };
	i64 getLFtimes() { return fm->getLFtimes(); };
	i64 getLocatetimes() { return fm->Locatetimes; };
	double gettotalruns(){return fm->gettotalruns();}
	vector<i64> getencoderadio(){return fm->getencoderadio();}
	// i64 GetEnterPosRank(i) { fm->GetEnterPosRank(i ); };
};

#endif
