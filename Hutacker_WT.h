/*============================================
# Filename: Hutacker_WT.h
# Ver 1.0 2014-06-08
# Copyright (C) 2014 ChenLonggang (chenlonggang.love@163.com)
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 or later of the License.
#
# Description: Hutacker-shape WT tree
=============================================*/
#ifndef HUTACKER_FM_H
#define HUTACKER_FM_H
#include"ABS_WT.h"
typedef struct hutaNode_t
{
	unsigned int freq;
	unsigned char label;
	int level;
	struct hutaNode_t * leftChild;
	struct hutaNode_t * rightChild;
}hutaNode_t;

typedef hutaNode_t * hutackerTree;

class Hutacker_FM : public ABS_FM
{
	public:
		Hutacker_FM(const char * filename,int block_size=1024,int D=32):ABS_FM(filename,block_size,D){}
		Hutacker_FM():ABS_FM(){}
		~Hutacker_FM(){};
	protected:
			int TreeCode();
	private:
			hutackerTree CreateHutackerTree();
			int HutackerNodesInit(hutaNode_t ** hutNodesPPtr);
			int FindMiniTwoNodes(hutaNode_t ** hutNodesPPtr,int nNodes,int * index1,int * index2);
			int MergeNodes(hutaNode_t ** hutaNodesPPtr,int index1,int index2,int nNodes);
			int GenerateHutackerCode(hutackerTree root);
			int SingleCharHutackerCode(hutackerTree root);
			int MultiCharHutackerCode(hutackerTree root);
			int DestroyHutackerTree(hutackerTree root);
			int GetHutackerTreeHeight(hutackerTree root);
			int ComputerCharDepth(hutackerTree root,int * charDepth);

};
#endif
