#include"loadtree.h"

int Loadlabel(loadkit & s,huffmanTree node)
{
	s.loadu8(node ->label);
	std :: cout << "Load label:" << node ->label << std :: endl;
	s.loadu8array((unsigned char*)node ->code,CODE_MAX_LEN);
    return 0;
}
int LoadNodeData(huffmanTree r,loadkit &s)
{
	if(!r)
		return 1 ;
	Loadlabel(s,r);
	LoadNodeData(r->leftChild,s);
	LoadNodeData(r->righChild,s);
	return 0;
}
huffmanTree LoadWTTree(loadkit &s,u32 alphabetsize)
{

	int nodecount = 2*alphabetsize -1; // the number of nodes of the wavelettree

	int * p = new int[nodecount];
	s.loadi32array(p,nodecount);
	map<int,huffmanTree> pmap;
	for(int i=0;i<nodecount;i++)
	{
		huffmanTree r = (huffmanTree)malloc(sizeof(huffNode_t));
		Loadlabel(s,r);
		pmap[p[i]] = r;
	}

	map<int ,huffmanTree>::iterator iter;
	map<int ,huffmanTree>::iterator f_iter;
	for(iter = pmap.begin();iter!=pmap.end();iter++)//build tree using map
	{
		f_iter = pmap.find(2*iter->first);
		if(f_iter != pmap.end())
			//iter->second->Left(f_iter->second);
			iter->second -> leftChild = f_iter -> second;
		else
			iter->second -> leftChild = NULL;

		f_iter = pmap.find(2*iter->first +1);
		if(f_iter!=pmap.end())
			iter->second-> righChild = f_iter -> second;
		else
			iter->second -> righChild = NULL;
	}
	huffmanTree root = NULL;
	f_iter = pmap.find(1);
	if(f_iter != pmap.end())
		root = f_iter->second;
	else
	{
		cerr<<"Load WTTree error"<<endl;
		 huffmanTree root = NULL;
		exit(0);
	}
	//LoadNodeData(root,s);
	return root;
}