/*============================================
# Filename: BaseClass.h
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description:
=============================================*/

#ifndef _MyCSA_IntClass
#define _MyCSA_IntClass
#include <string>
#include <unistd.h>
#include <getopt.h>
#define CODE_MAX_LEN 256
#define CHAR_SET_SIZE 256

#define u64 unsigned long long int
#define su64 static unsigned long long int
#define i64 long long int
#define si64 static unsigned long long int
#define u32 unsigned int
#define su32 static unsigned int
#define i32 int
#define si32 static int
#define u16 unsigned short
#define su16 static unsigned short
#define i16 short
#define si16 static short
#define uchar unsigned char
#define u8 unsigned char
using namespace std;
extern int seqlength;
extern bool SAVESAL;

enum SampleMethod
{
    ordinary,    /* 最早的采样 */
    equidistant, /* 平均采样 */
    Uniform,     /* 最优采样 */
    Uniformmy,    /*更改weight公式*/
    hash
};
// static const char *optString = "d:f:r:s:b:t:p:lceohm:a?";
// static const struct option longOpts[] = {
//     {"coding", required_argument, NULL, 'd'},
//     {"blocksize", required_argument, NULL, 'b'},
//     {"samplerate", required_argument, NULL, 's'},
//     {"runtimes", required_argument, NULL, 't'},
//     {"filename", required_argument, NULL, 'f'},
//     {"randomseed", required_argument, NULL, 'r'},
//     {"samplemethod", required_argument, NULL, 'm'},
//     {"patternlen", required_argument, NULL, 'p'},
//     {"runCount", no_argument, NULL, 'c'},
//     {"runLocate", no_argument, NULL, 'l'},
//     {"runExtract", no_argument, NULL, 'e'},
//     {"output", no_argument, NULL, 'o'},
//     {"calsample", no_argument, NULL, 'a'},
//     {"help", no_argument, NULL, 'h'},
//     {NULL, no_argument, NULL, 0}};
static const char *optString = "v:z:d:f:r:s:b:t:p:i:lceohm:a?";
static const struct option longOpts[] = {
    {"mode", required_argument, NULL, 'z'},
    {"speedlevel", required_argument, NULL, 'v'},
    {"coding", required_argument, NULL, 'd'},
    {"blocksize", required_argument, NULL, 'b'},
    {"InputPattern", required_argument, NULL, 'i'},
    {"samplerate", required_argument, NULL, 's'},
    {"runtimes", required_argument, NULL, 't'},
    {"filename", required_argument, NULL, 'f'},
    {"randomseed", required_argument, NULL, 'r'},
    {"samplemethod", required_argument, NULL, 'm'},
    {"patternlen", required_argument, NULL, 'p'},
    {"runCount", no_argument, NULL, 'c'},
    {"runLocate", no_argument, NULL, 'l'},
    {"runExtract", no_argument, NULL, 'e'},
    {"output", no_argument, NULL, 'o'},
    {"calsample", no_argument, NULL, 'a'},
    {"help", no_argument, NULL, 'h'},
    {NULL, no_argument, NULL, 0}};


enum MODE
{
    // no_argument,
    build,    /* */
    search,   /* 平均采样 */
    eqsearch, /* */
    bindex,     /* 索引构建*/
    cr,
    lr,
    er,
    efq,
    help
};


// enum IDMODE
// {
//     // no_argument,
//     fix, /* */
//     IA,  /* 平均采样 */
//     FM
// };

struct globalArgs_t
{
    int MODE;/* -z */
    string INPUTPATTERN;/* -i */
    int CODING;       /*  -d */
    int BLOCKSIZE;    /*  -b */
    int RUNTIMES;     /*  -t */
    int SAMPLERATE;   /* -s */
    int RANDOMSEED;   /* -r */
    int SAMPLEMETHOD; /* -m */
    int PATTERNLEN;
    string FILENAME; /* -f */
    string PATTERNFILE; /* -p */
    string OUTFILE;
    bool runCount;    /* -c */
    bool runLocate;   /* -l */
    bool runExtract;  /* -r */
    bool output;      /* -o */
    bool calsample;      /* -o */
    int speedlevel;
};
extern globalArgs_t globalArgs;

#endif
