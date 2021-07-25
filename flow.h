/*
 * @Author: your name
 * @Date: 2021-05-30 15:33:07
 * @LastEditTime: 2021-07-19 15:21:55
 * @LastEditors: Please set LastEditors
 * @Description: In User Settings Edit
 * @FilePath: /CIndex-master/flow.h
 */
#ifndef FLOW_H
#define FLOW_H
#include"BaseClass.h"
#include<iostream>
#include<fstream>
#include<vector>
#include<string>
#include<stdio.h>
#include<time.h>
#include<omp.h>
#include"statics.h"
#include "FM.h"
#include "InArray.h"
#include"IDcmp.h"
#include"CreateText.h"
#include"reverseFile.h"
#include<tuple>
#include<sys/time.h>


const string SAL = ".SAL";
class FTcomp{
    public:
        void paraCMPDNA(int seqlength,int blockSize, int shape, std :: string savePath,int dna__len);
        void paraCMPID(std::vector<std::string>ID,int blockSize,int sample, int shape, std :: string savePath);
        void compFastqFile(std ::string fastqfile, std ::string savePath);
        void paraCMPQV(int blockSize, int shape, std ::string savePath);
        void saveID(std ::string cmpid);
        void LoadIndexFromCompressedFile(std ::string cmpid, std ::string cmpdna, std ::string cmpqv, std ::string savePath);
        FM *buildIndexFromCmpfile(string cmpDNA, string savePath);
        FTcomp(){builded = false;};
    public:
        void GetFASTQ(string pattern, vector<string> &result); 
        i64*   GetDnaPatternLine(string pattern,i64& num);
        i64  GetDnaPatternCount(string pattern);
        string DNAextract(u64 lineno);
        void GetIDindex(string cmpid, string savePath);
        void DeleteSameLine3(char D, i64* dnaposvec,i64 num) ;
        void GetID(int index,string& strID);
        void LoadIDSAL(string cmpid);
        double gettotalruns(){return fmdna->gettotalruns();}
        i64 getreadssize(){return fmdna->SizeInByte();} 
        i64 getqvsize(){return fmqua->SizeInByte();} 
        i64 getidsize()
        {
            i64 idsize = 0;
            for(int i = 0;i < v_FMid.size();++i)
                if(v_FMid[i] != 0)
                    idsize += v_FMid[i]->SizeInByte();
            for(int i = 0;i < v_IAid.size();++i)
                if(v_IAid[i] != 0)
                    idsize += v_IAid[i]->GetMemorySize();
            for(int i = 0;i < v_strid.size();++i)
                idsize += v_strid[i].length();
            idsize  +=  v_strsplitid.size();
            return idsize;
        }
        i64 gettotalsize(){return getidsize() + getqvsize() + getreadssize();}
        i64 getreadscountsize(){return fmdna->SizeInByte_count();}
        i64 getreadslocatesize(){return fmdna->SizeInByte();}
        i64 gettextsize(){return fmdna->SizeInText();}
        vector<i64> getreadsencoderadio(){return fmdna->getencoderadio();}
      private:
        bool builded;
        vector<string> IDS;
        FM *fmdna;
        FM *fmqua;
        vector<FM*>       v_FMid;
        vector<InArray *> v_IAid;
        vector<string>    v_strid;
        vector<char> v_strsplitid;
        vector<int> v_pos;
        vector<int> v_length;
        char *ftcomp_first_id_seq;
        bool dna_fix_len;
        i64 dna_len;
};
#endif

