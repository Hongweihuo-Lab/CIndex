#include"BaseClass.h"

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
int seqlength;
bool SAVESAL;
// globalArgs_t globalArgs;
globalArgs_t globalArgs;

// void help()
// {
// 	cout<<"--coding or -v :coding method ;0=runlength,1=elias-fano coding,default is 0"<<endl;
// 	cout<<"--blocksize or -b:blocksize;default is 64"<<endl;
// 	cout<<"--samplerate or -s:default is 32"<<endl;
// 	// cout<<
// }