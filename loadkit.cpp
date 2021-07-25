/*============================================
# Filename: loadkit.cpp
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description:
=============================================*/
#include"loadkit.h"
void loadkit::close()
{
	if(r!=NULL)
		fclose(r);
	r=NULL;
}

loadkit::~loadkit()
{
	if(r!=NULL)
		fclose(r);
}

loadkit::loadkit(const char * file)
{
	this->r=fopen(file,"rb");
	if(r==NULL)
	{
		cout<<file<<" Fopen error"<<endl;
		exit(0);
	}
}

i32 loadkit::loadi64(i64 & value)
{
	i32 num=fread(&value,sizeof(i64),1,r);
	return num;
}
i32 loadkit::loadu64(u64 & value)
{
	i32 num = fread(&value, sizeof(char), 8, r);
	return num;
}
i32 loadkit::loadi32(i32 & value)
{
	i32 num=fread(&value,sizeof(i32),1,r);
	return num;
}
i32 loadkit::loadu32(u32 & value)
{
	i32 num=fread(&value,sizeof(u32),1,r);
	return num;
}
i32 loadkit::loadi16(i16 & value)
{
	i32 num=fread(&value,sizeof(i16),1,r);
	return num;
}
i32 loadkit::loadu16(u16 & value)
{
	i32 num=fread(&value,sizeof(u16),1,r);
	return num;
}
i32 loadkit::loadu8(u8 & value)
{
	i32 num = fread(&value,sizeof(u8),1,r);
	return num;
}

i32 loadkit::loadi64array(i64 * value,i32 len)
{
	i32 num=fread(value,sizeof(i64),len,r);
	return num;
}
i32 loadkit::loadu64array(u64 * value,i32 len)
{
	i32 num=fread(value,sizeof(u64),len,r);
	return num;
}
i32 loadkit::loadi32array(i32 * value,i32 len)
{
	i32 num=fread(value,sizeof(i32),len,r);
	return num;
}
i32 loadkit::loadu32array(u32 * value,i32 len)
{
	i32 num=fread(value,sizeof(u32),len,r);
	return num;
}
i32 loadkit::loadi16array(i16 * value,i32 len)
{
	i32 num=fread(value,sizeof(i16),len,r);
	return num;
}
i32 loadkit::loadu16array(u16 * value,i32 len)
{
	i32 num=fread(value,sizeof(u16),len,r);
	return num;
}

i32 loadkit::loadu8array(u8 * value,i32 len)
{
	i32 num = fread(value,sizeof(u8),len,r);
	return num;
}












