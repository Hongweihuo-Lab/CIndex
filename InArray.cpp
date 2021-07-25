/*============================================
# Filename: InArray.cpp
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description:
=============================================*/
#include "InArray.h"
#include<string.h>
#include<stdlib.h>
#include<iostream>
#include<math.h>
using namespace std;
InArray::~InArray(void)
{
	delete [] data;
}
InArray::InArray()
{
	this->datanum = 0;
	this->datawidth = 0;
}
InArray::InArray(u64 data_num, u32 data_width)
{
	if(data_num<=0||data_width<=0)
		cout<<"InArray����������������"<<endl;
	else
	{
		this->datanum =data_num;
		this->datawidth =data_width;
	    u64 totlesize=datanum*datawidth;
		if(totlesize%32==0)
			totlesize=totlesize/32+1;
		else
			totlesize=(totlesize/32)+2;
		this->data =new u32[totlesize+1];//valgrand warns
		memset(data,0,4*totlesize);
		//mask=((1<<datawidth)-1);
		mask = (u64)pow(2,datawidth)-1;
	}
}

//�������±�Ϊindex��λ������ֵΪvalue
void InArray::SetValue (u64 index, u64 v)
{

	if(index>datanum-1|| index<0)
	{
		cerr<<"InArray:index out of boundary:"<<v<<endl;

		//__LINE__ exit(0) ;
	}
	else if(v>mask)
	{
		cerr<<"InArray:value:"<<v<<" is out of boundary"<<endl;
		//__LINE__ exit(0) ;
	}
	else
	{
		u64 value=v;
		u64 anchor=(index*datawidth)>>5;
		u64 temp1=data[anchor];
		u32 temp2=data[anchor+1];
		u32 temp3=data[anchor+2];
		temp1=(temp1<<32)+temp2;
		i32 overloop=((anchor+2)<<5)-(index+1)*datawidth;
		if(overloop<0)
		{
//01 000000000000000000000000000000000 00
			i32 overloop2 = datawidth + overloop -32;
			value=(value>>(-overloop));//35
			temp1=temp1+value;
			data[anchor+1]=(temp1&(0xffffffff));
			data[anchor]=(temp1>>32)&(0xffffffff);
			data[anchor+2]=(v<<(32+overloop))&(0xffffffff);
		}
		else
		{
			value=(value<<overloop);
			temp1=temp1+value;
			data[anchor+1]=(temp1&(0xffffffff));
			data[anchor]=(temp1>>32)&(0xffffffff);
		}

	}


}

u64 InArray::GetNum ()
{
	return datanum;
}
u64 InArray::GetMemorySize()
{
	return (datanum*datawidth)/8;//wch
}


u32 InArray::GetDataWidth()
{
	return datawidth;
}

u64 InArray::GetValue(u64 index)
{
	if(index>datanum-1||index<0)
	{
		cerr<<"InArray:GetValue: index out of boundary"<<endl;
		exit(0);
	}

	u64 anchor=(index*datawidth)>>5;
	u64 temp1=data[anchor];
	u32 temp2=data[anchor+1];
	u32 temp3=data[anchor+2];
	i32 overloop=((anchor+2)<<5)-(index+1)*datawidth;
	temp1=(temp1<<32)+temp2;
	if(overloop<0)
	{
	   // i32 overloop2 = datawidth + overloop - 32;
	   //int temp =0-overloop;
	   temp1 = (temp1<<(-overloop))+(temp3>>(32+overloop));
	    //value = (value >> (-overloop));
		return temp1&mask;
	}
	else
	{

		return (temp1>>overloop)&mask;
	}
}
i64 InArray::write(savekit & s)
{
	// cout<<__FILE__<<__LINE__<<":"<<s.pos()<<endl;
	s.writeu64(datanum);
	// cout<<__FILE__<<__LINE__<<":"<<s.pos()<<endl;
	s.writeu32(datawidth);
	u64 len=(datanum*datawidth);
	if(len%32==0)
		len=len/32+1;
	else
		len=len/32+2;

	s.writeu64(len);
	// cout<<__LINE__<<":Inarray:"<<len<<endl;
	s.writeu32array(data,len);
	return 1;
}
i64 InArray::load(loadkit & s)
{
	// cout<<__FILE__<<__LINE__<<":"<<s.pos()<<endl;
	s.loadu64(datanum);
	// cout<<__FILE__<<__LINE__<<":"<<s.pos()<<endl;
	s.loadu32(datawidth);
	u64 len=0;
	s.loadu64(len);
	//len++;//valgrind warns
	// cout<<__LINE__<<":Inarray:"<<len<<endl;
	//cout<<len<<endl;
	data=new u32[len+1];
	s.loadu32array(data,len);
	mask=(u64)pow(2,datawidth)-1;
	return 1;
}



