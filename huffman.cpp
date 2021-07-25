#include<iostream>
#include"huffman.h"
#include<iostream>
int getBlockDataInfo(unsigned char *inbuff, unsigned int len, bool *charMap,
	unsigned int *charFreq, unsigned int* setSiz)
{
	if (!inbuff || !charMap || !charFreq)
	{
		return -1;
	}
	unsigned int i;
	for (i = 0; i<CHAR_SET_SIZE; i++)
	{
		charFreq[i] = 0;
	}
	for (i = 0; i<len; i++)
	{
		charFreq[inbuff[i]]++;
	}
	*setSiz = 0;
	for (i = 0; i<CHAR_SET_SIZE; i++)
	{
		if (charFreq[i])
		{
			(*setSiz)++;
			charMap[i] = true;
		}
		else{
			charMap[i] = false;
		}
	}
	return 0;
}

void initHuffmanNode(huffmanTree node, unsigned char label, unsigned int freq)
{
	node->freq = freq;
	node->label = label;
	memset(node->code, 0, CODE_MAX_LEN);
	node->leftChild = NULL;
	node->righChild = NULL;
	node->next = NULL;
}
huffmanTree mergeTwoMiniNode(huffmanTree head)
{
	huffmanTree nextPtr = head;
	huffmanTree nextPtr_p = head;

	unsigned int max = -1;
	huffmanTree node1 = NULL;
	while (nextPtr)
	{//找
		if (nextPtr->freq < max)
		{
			node1 = nextPtr;
			max = nextPtr->freq;
		}
		nextPtr = nextPtr->next;
	}
	nextPtr = head;
	while (nextPtr != node1)
	{
		nextPtr_p = nextPtr;
		nextPtr = nextPtr->next;
	}
	if (nextPtr != head)
		nextPtr_p->next = nextPtr->next;
	else
		head = head->next;
	node1->next = NULL;
	

	max = -1;
	huffmanTree node2 = NULL;
	nextPtr = head;
	while (nextPtr)
	{//找
		if (nextPtr->freq < max)
		{
			node2 = nextPtr;
			max = nextPtr->freq;
		}
		nextPtr = nextPtr->next;
	}
	nextPtr = head;
	while (nextPtr != node2)
	{
		nextPtr_p = nextPtr;
		nextPtr = nextPtr->next;
	}
	if (nextPtr != head)
		nextPtr_p->next = nextPtr->next;
	else
		head = head->next;
	node2->next = NULL;

	huffmanTree huffmanNode = (huffmanTree)malloc(sizeof(huffNode_t));
	initHuffmanNode(huffmanNode, '\0', node1->freq+node2->freq);
	huffmanNode->next = head;
	huffmanNode->leftChild = node1;
	huffmanNode->righChild = node2;
	return huffmanNode;
}
huffmanTree createHuffman(unsigned char *L, unsigned int len, dataInfo *dInfoPtr)
{
	int ret;
	ret = getBlockDataInfo(L, len,	dInfoPtr->charMap,	dInfoPtr->charFreq,	&dInfoPtr->setSize);

	//初始化链表
	huffmanTree huffmanHead = NULL;
	huffmanTree nextPtr = NULL;
	char currLab = 0;

	for (int i = 0; i < dInfoPtr->setSize; i++)
	{
		if (i == 0)
		{
			huffmanHead = (huffmanTree)malloc(sizeof(huffNode_t));
			while (dInfoPtr->charMap[currLab] == false)currLab++;
			initHuffmanNode(huffmanHead, currLab, dInfoPtr->charFreq[currLab]);
			currLab++;
			nextPtr = huffmanHead;
		}
		else{
			nextPtr->next = (huffmanTree)malloc(sizeof(huffNode_t));
			while (dInfoPtr->charMap[currLab] == false)currLab++;
			initHuffmanNode(nextPtr->next, currLab, dInfoPtr->charFreq[currLab]);
			currLab++;
			nextPtr = nextPtr->next;
		}
	}
	int nodes = dInfoPtr->setSize;
	//建立huffman树的过程
	while (nodes-- > 1)
	{
		huffmanHead = mergeTwoMiniNode(huffmanHead);
	}
	return huffmanHead;
}
void generateCodeTable(huffmanTree root, char codeTable[CHAR_SET_SIZE][CODE_MAX_LEN], char code[CODE_MAX_LEN],char nextCode,int level)
{
	if (root->leftChild == NULL&&root->righChild == NULL)
	{
		for (int i = 0; i < 256; i++)
		{
			codeTable[root->label][i] = code[i];
		}
		codeTable[root->label][level-1] = nextCode;
		int xxxx = 0;
		xxxx++;
		return;
	}
	if (level != 0)
	{
		for (int i = 0; i < 256; i++)
		{
			root->code[i] = code[i];
		}
		root->code[level-1] = nextCode;
	}
	if (root->leftChild)
		generateCodeTable(root->leftChild, codeTable, root->code, '0', level + 1);
	if (root->righChild)
		generateCodeTable(root->righChild, codeTable, root->code, '1', level + 1);
	return;
}


void destroyHuffmanTree(huffmanTree root)
{
	if (root->leftChild == NULL&&root->righChild == NULL)
	{
		delete root;
		root = NULL;
		return;
	}
		
	if (root->leftChild != NULL)
	{
		destroyHuffmanTree(root->leftChild);
	}
	if (root->righChild != NULL)
	{
		destroyHuffmanTree(root->righChild);	
	}
	if (root)
	{

		delete root;
		root = NULL;
	}
	
}
void printCodeTable(dataInfo dinfo)
{
	for(int i = 0; i < 256; i++)
	{
		if(dinfo.charMap[i])
		{
			cout << i << " : ";
			int j = 0;
			while(dinfo.codeTable[i][j] != 0)
			{
				cout << dinfo.codeTable[i][j];
				j++;
			}
			cout << endl;
		}
	}
	return;
}
void printData(u8* data,int k)
{
	for(int i = 0; i < k; i++)
	{
		int ans = data[i];
		std :: cout << ans << std :: endl;
	}
	return;
}
void AppendHuff(unsigned char *data, unsigned char x, unsigned long long &curbit, dataInfo info)
{
	int i = 0;
	unsigned char ch = info.codeTable[x][i++];
	while(ch != 0 && i < 256)
	{
		if(ch == '0')curbit++;
		else if(ch == '1')
		{
			unsigned long long index = curbit / 8;
			int offset = curbit % 8;
			data[index] |= (1 << (8 - 1 - offset));
			curbit++;
		}
		else
		{
			std :: cout << "error: " << ch << std :: endl;
		}
		ch = info.codeTable[x][i++];
	}
	return;
}
u32 HuffmanCode(unsigned char* input,unsigned long long n,std :: string outputfile,std :: string dic,u64 bwtIndex,int *C)
{
	std :: cout << "Huffman code.....\n";
	unsigned char *data = new unsigned char [n];
	memset(data,0,n);
	//unsigned long long Len = n + 1;
	dataInfo dInfo;
	memset(dInfo.codeTable, '/0', 256 * 256);
	huffmanTree huffmanRoot=createHuffman(input, n, &dInfo);
	generateCodeTable(huffmanRoot, dInfo.codeTable, huffmanRoot->code, '1', 0);
	printCodeTable(dInfo);
	
	unsigned long long curbit = 0;
	unsigned char ch;
	for(unsigned long long i = 0; i < n; i++)
	{
		ch = input[i];
		AppendHuff(data,ch,curbit,dInfo);
	}
	unsigned long long int numbytes;
	if(curbit % 8 == 0)
		numbytes = curbit / 8;
	else
		numbytes = curbit / 8 + 1;
	
	savekit *savefile = new savekit(outputfile.c_str()); 
	
	//n
	savefile -> writeu64(n);
	//setSize
	savefile -> writeu32(dInfo.setSize);
	
	//savetree
	SaveWTTree(*savefile,huffmanRoot);

	//numbytes
	savefile ->writeu64(numbytes);
	//data
	savefile ->writeu8array(data,numbytes);
	delete [] data;

	int diclen = dic.size();
	unsigned char * dic_array = new uchar [diclen];
	for(int i = 0; i < diclen; i++)
		dic_array[i] = dic[i];
	//diclen
	savefile ->writei32(diclen);
	//dic_array
	savefile ->writeu8array(dic_array,diclen);
	delete [] dic_array;dic_array = NULL;
	//C
	savefile ->writei32array(C,256);
	//bwtIndex
	savefile ->writeu64(bwtIndex);

	destroyHuffmanTree(huffmanRoot);
	std :: cout << "Huffman coding complete: " << numbytes / 1024 << "KB.\n";
	int sizekb =  numbytes / 1024;
	delete savefile;
	return sizekb;
}
int getbit(u8*data, u64 decodebit)
{
	int ans = 0;
	u64 index = decodebit / 8;
	int offset = decodebit % 8;
	return (data[index] & (1 << (8 - offset - 1)));
}

int genResultByTree(int *result,u64 n,u8 *data,huffmanTree root)
{
	u64 decodeBit = 0;
	u64 decodenum = 0;
	huffmanTree curnode = root;
	while(decodenum < n)
	{
		while(curnode ->leftChild != NULL && curnode ->righChild != NULL)
		{
			if(getbit(data,decodeBit) > 0)
				curnode = curnode ->righChild; 
			else
				curnode = curnode ->leftChild;
			decodeBit++;
		}
		if(curnode ->leftChild == NULL && curnode ->righChild == NULL)
		{
			int ans = curnode ->label;
			result[decodenum++] = ans;
			curnode = root;
		}
		else
		{
			std :: cout << "genResultByTree Error.\n";
		}
	}
	return 0;
}

int* HuffmanDecode(std :: string inputfile,std :: string &dic,int *C,u64 &bwtIndex,unsigned long long &n)
{
	std :: cout << "Huffman decoding....\n";
	loadkit *loadfile = new loadkit(inputfile.c_str());
	dataInfo dInfo;
	memset(dInfo.codeTable, '/0', 256 * 256);
	loadfile ->loadu64(n);
	loadfile ->loadu32(dInfo.setSize);
	
	//loadtree
	huffmanTree root = LoadWTTree(*loadfile, dInfo.setSize);

	unsigned long long int numbytes = 0;
	//numbytes
	loadfile ->loadu64(numbytes);
	unsigned char* data = new unsigned char [numbytes];
	//data
	loadfile ->loadu8array(data,numbytes);

	//dic
	int diclen = 0;
	loadfile ->loadi32(diclen);
	uchar* dic_array = new uchar [diclen];
	//dicarray
	loadfile ->loadu8array(dic_array,diclen);
	for(int i = 0; i < diclen; i++)
		dic.push_back(dic_array[i]);
	//C
	loadfile ->loadi32array(C,256);
	//bwtIndex
	loadfile ->loadu64(bwtIndex);

	int * result = new int [n];
	genResultByTree(result,n,data,root);
	delete [] data;
	std :: cout << "Huffman decode complete\n";
	return result;
}