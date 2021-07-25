#include"statics.h"
#include<fstream>
#include<iostream>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

unsigned char buf[1024*1024]; /* 1MB buffer */
/* 截取文件iname的前 n MB 个字节 */
void cut_file(char const *iname, int n)
{
    char oname[1024];
    FILE *ifile,*ofile;

    if (n <= 0) {
	fprintf(stderr,"So <mb> must be positive!\n");
	exit(EXIT_FAILURE);
    }

    /* 输出文件名, 和输入文件同属一个目录*/
    sprintf(oname,"%s.%iMB",iname,n);

    /* fopen64 为linux 特有, 可以打开超过2GB的文件 */
    if (!(ifile = fopen(iname,"r"))) {
        perror(iname);
        exit(EXIT_FAILURE);
    }
    
    if (!(ofile = fopen(oname,"w"))) {
        perror(oname);
        exit(EXIT_FAILURE);
    }

    while (n--) {
	if (fread (buf,1024*1024,1,ifile) != 1) { /* 每次读1MB */
	    fprintf(stderr,"Error: cannot read %s\n",iname);
	    fprintf(stderr, "%s\n", strerror(errno));
	    if (errno == 0)
		fprintf(stderr,"Maybe the file is too short?\n");
	    fclose (ifile); fclose (ofile);
	    unlink (oname); 	/* 删除oname文件*/
	    exit(EXIT_FAILURE);
	}
	if (fwrite (buf,1024*1024,1,ofile) != 1) { /* 每次写1MB */
	    fprintf(stderr,"Error: cannot write %s\n",oname);
	    fprintf(stderr, "%s\n", strerror(errno));
	    fclose (ifile); fclose (ofile);
	    unlink (oname);
	    exit(EXIT_FAILURE);
	}
    }

    fclose (ifile);fclose(ofile);
    
    printf("Successfully created %s.\n",oname);
}
void trimFile(std :: string FILE,std :: string outfile)
{
	std :: vector<std :: string>s;
	std :: ifstream fin;
	fin.open(FILE,std :: ios_base :: in);
	if(!fin.is_open())
	{
		std :: cout << "Can't open file:" << FILE << std :: endl;
		exit(-1);
	}
	std :: ofstream fout; 
	fout.open(outfile,std :: ios_base :: out | std :: ios_base :: trunc);
	if(!fout.is_open())
	{
		std :: cout << "Can't open file:" << outfile << std :: endl;
		exit(-1);
	}
	unsigned long long int row = 0;
	std :: string seq;
	while(getline(fin,seq))
	{
		if(seq.size()>1)
		{row++;
		s.push_back(seq);
		}
	}
	int r = row % 4;
	row -= r;
	//fin.close();
	unsigned long long i = 0;
	while(i < row)
	{
		fout << s[i] << std :: endl;
		i++;
	}
	fin.close();
	fout.close();
	return;
}

int *cmp_chFreq(std :: vector<std :: string>QV, int *C)
{
	int table[256] = {0};
	
	for(int i = 0; i < QV.size(); i++)
	{
		int len = QV[i].size();
		bool flag = true;
		for(int j = 0; j < len; j++)
		{
			if(QV[i][j] == ' ')
			{
				flag = false;
				continue;
			}
			if(!flag)
			{
				int value = QV[i][j];
				table[value]++;
			}
		}
		
	}
	int QVsize = 0;
	for(int i = 0; i < 256; i++)
		{if(table[i] != 0)
			QVsize++;
	}
	//按频率排序
	for(int i = 1; i < 256; i++)
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
	printf("按字符频率排名--C表:\n");
	for(int i = 0; i < QVsize; i++)
	{printf("%c %d\n",C[i],table[C[i]]);}
	return C;
}