#include"saveTree.h"

int Savelabel(savekit &s, huffmanTree node)
{
	s.writeu8(node ->label);
	std :: cout << "Sava label:" << node ->label << std :: endl;
	s.writeu8array((unsigned char*)(node -> code),CODE_MAX_LEN);
	return 0;
}
int SaveWTTree(savekit &s,huffmanTree root)
{
	//保存编号信息
	//int nodecount = 2*alphabetsize -1;
	//s.writei32(nodecount);
	SaveNodePosition(root,1,s);

	//保存节点数据信息
	SaveNodeData(root,s);
	return 0;
}
int SaveNodeData(huffmanTree r,savekit &s)
{
	if(!r)
		return 1 ;
	Savelabel(s,r);
	SaveNodeData(r->leftChild,s);
	SaveNodeData(r->rightChild,s);
	return 0;
}
int SaveNodePosition(huffmanTree r,u32 position,savekit &s)
{
	if(!r)
		return 1;
	s.writei32(position);
	SaveNodePosition(r->leftChild, 2 * position, s);
	SaveNodePosition(r->rightChild, 2 * position +1, s);
	return 0;
}