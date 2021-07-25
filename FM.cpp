/*============================================
# Filename: FM.cpp
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description:
=============================================*/
// #include"FM.h"
#include"FM.h"
#include"Huffman_WT.h"
#include"Balance_WT.h"
#include"Hutacker_WT.h"

FM::FM() : fm(new ABS_FM()), u() {}

FM::FM(const char *filename, int block_size, int D, int shape, int speedlevel,int method, bool calsample )
{
	if(block_size<=0 || shape<0 || shape >2)
	{
		cout<<"FM::FM error parmater"<<endl;
		exit(0);
	}
	mtd = method;
	 switch (shape)
	{
	case 0:
		fm = new Hutacker_FM(filename, block_size, D);
		// cout << "-------Build Hutacker-------" << endl;
		break;
	case 1:
		fm = new Huffman_FM(filename, block_size, D);
		// cout << "--------Build Huffman-------" << endl;
		break;
	case 2:
		fm = new Balance_FM(filename, block_size, D);
		// cout << "--------Build Balance--------" << endl;
		break;
	default:
		fm = new Hutacker_FM(filename, block_size, D);
		// cout << "--------Build Hutacker--------" << endl;
		break;
	}
	fm->BuildTree(speedlevel, method, calsample);
}

FM::FM(const FM &h):fm(h.fm),u(h.u){}
int FM::Save(const char *indexfile)
{
	savekit s(indexfile);
	s.writeu64(198809102510);
	fm->Save(s);
	s.close();

	return 0;
}
int FM::Save_qv(const char *indexfile)
{
	savekit s(indexfile);
	s.writeu64(198809102510);
	fm->Save(s);
//	fm->SaveSAL(s,0);
	s.close();

	return 0;
}
int FM::SaveSAL(const char *indexfile, char savemode)
{
	savekit s(indexfile);
	s.writeu64(198809102510);
	fm->SaveSAL(s, savemode);
	s.close();

	return 0;
}
 double FM::compressRatio()
{
	// auto aa = SizeInByte();
	// cout<< __LINE__ << aa;
	return SizeInByte() / (GetN() * 1.0);
}
int FM::LoadSAL(const char *indexfile)
{
	loadkit s(indexfile);
	unsigned long long int magicnum = 0;
	s.loadu64(magicnum);
	if (magicnum != 198809102510)
	{
		cerr << "Not a FM_Index file" << endl;
		exit(0);
	}
	mtd=fm->LoadSAL(s);
	s.close();
	return 0;
}
int FM::Load(const char *indexfile)
{
	loadkit s(indexfile);
	unsigned long long int magicnum = 0;
	s.loadu64(magicnum);
	if (magicnum != 198809102510)
	{
		cerr << "Not a FM_Index file" << endl;
		return 1;
	}
	fm->Load(s);
	s.close();
	return 0;
}
FM & FM::FM:: operator =(const FM & h)
{
	if(u.reattach(h.u))
	{
		delete fm;
	}
	fm = h.fm;
	return * this;
}

FM::~FM()
{
	if(u.only())
		delete fm;
}


