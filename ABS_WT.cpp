/*============================================
# Filename: ABS_WT.cpp
# Ver 2.0 2019-06-09
# Copyright (C) 2019 Chenhui Wang, Pengfei Liu, Hongwei Huo, Jeffrey Scott Vitter
#
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License (GPLv3.0) or later of the License.
#
# Description:
=============================================*/
#include <limits>
#include <math.h>
#include <iterator>
#include "ABS_WT.h"
//#include"ds_ssort.h"
#include <string.h>
#include <map>
#include <iomanip>
#include <vector>
#include <thread>
#include <sys/shm.h>
#include <error.h>
#include <string.h>
#include <sys/wait.h>
#include <math.h>
#include <thread>
#include <algorithm>
//#define LOOP 35
#define SIZE 1024

#define READSIZE 1024 * 1024 * 200
i64 GetDenbihe(i64 bi, i64 len)
{
	return (((i64)pow(bi, len + 1)) - bi) / (bi - 1);
}
u64 GetBits(u64 *buff, i64 &index, int bits)
{
	if ((index & 0x3f) + bits < 65)
		return (buff[index >> 6] >> (64 - ((index & 0x3f) + bits))) & ((0x01ull << bits) - 1);
	int first = 64 - (index & 0x3f);
	int second = bits - first;
	u64 high = (buff[index >> 6] & ((0x01ull << first) - 1)) << second;
	return high + (buff[(index >> 6) + 1] >> (64 - second));
}
int fmablog(i64 x)
{
	int ans = 0;
	while (x > 0)
	{
		ans++;
		x = x >> 1;
	}
	return ans;
}
int Zeros(u16 x, ABS_FM *t)
{
	if (t->Z[x >> 8] == 8)
		return t->Z[x >> 8] + t->Z[(uchar)x];
	else
		return t->Z[x >> 8];
}
i64 GammaDecode(u64 *buff, i64 &index, ABS_FM *t)
{
	u32 x = GetBits(buff, index, 32);
	int runs = Zeros(x >> 16, t);
	int bits = (runs << 1) + 1;
	index = index + bits;
	return x >> (32 - bits);
}

ABS_FM::ABS_FM(const char *filename, int block_size, int D)
{
	this->block_size = block_size;
	this->D = D;
	this->T = NULL;
	T = Getfile(filename);
	enterposflag = false;
	Inittable();
}
ABS_FM::~ABS_FM()
{
	DestroyWaveletTree();
	if (T)
		delete[] T;
	if (bwt)
		delete[] bwt;
	if (C)
		delete[] C;
	if (code)
		delete[] code;
	if (Z)
		delete[] Z;
	if (R)
		delete[] R;
	if (R1)
		delete[] R1;
	if (R3)
		delete[] R3;
}
Sampler *ABS_FM::CalSampler(int lengthOfPattern, saidx64_t *SA, bool originweightmethod)
{
	int charMount = C[0];
	string allAlpbet = "";
	for (int i = 0; i < 256; i++)
	{
		if (charFreq[i] > 0 && isalpha(i))
		{
			allAlpbet = allAlpbet + (char)i;
		}
	}
	// allAlpbet = "ACGTN";
	uchar Creal[256] = {0};
	int count = 0;
	for (int i = 0; i < allAlpbet.length(); i++)
	{
		Creal[allAlpbet[i]] = count++;
	}
	auto pos = AllPossiblePattern(allAlpbet, lengthOfPattern);
	vector<int> countOfPattern;
	i64 count_num = 0;
	for (auto i : pos)
	{
		Counting(i.c_str(), count_num);
		// Counting("TTGTA", count_num);
		countOfPattern.push_back(count_num);
	}
	FILE *fp = fopen(reinterpret_cast<const char *>(filename), "r");
	// cout<<filename<<endl;
	char *inputT = new char[n];
	fread(inputT, sizeof(char), n, fp);
	uint *outputWeight = new uint[n];
	memset(outputWeight, 0, n * sizeof(int));
	int startindex = GetDenbihe(allAlpbet.length(), lengthOfPattern - 1);
	int i = 0;
	for (i = 0; i < n - lengthOfPattern; i++)
	{
		int indexIncountOfPattern = 0;
		int mulnum = 1;
		for (int j = 0; j < lengthOfPattern; j++)
		{ //把ａｃｇｔｎ转换成对应的５进制数，结果即为下标
			indexIncountOfPattern += Creal[inputT[i + lengthOfPattern - j - 1]] * mulnum;
			mulnum *= allAlpbet.length();
		}
		if (originweightmethod)
			outputWeight[i] = countOfPattern[startindex + indexIncountOfPattern];
		else
			outputWeight[SA[i]] = countOfPattern[startindex + indexIncountOfPattern];
	}
	Sampler *sampler = 0;
	bool forward = false;
	WeightedSampler *wsampler = new WeightedSampler(outputWeight, n, forward);
	int threads = 1;
	if (!(wsampler->buildSamples(this->D, /*parameters.get(SAMPLE_RATE)*/ 0 /*parameters.get(INITIAL_ADJUSTMENT)*/, threads)))
	{
		delete wsampler;
		wsampler = 0;
		// return 5;
	}
#ifdef COUNTLFTIMES
	if (originweightmethod)
		testorigin(sampler, SA, originweightmethod, outputWeight);
	else
		testmy(sampler, SA, originweightmethod, outputWeight);
#endif //
	sampler = wsampler;
	wsampler = 0;
	///////
	return sampler;
	// cout<< allAlpbet << endl;
}
void ABS_FM::testorigin(Sampler *sampler, saidx64_t *SA, bool originweightmethod, uint *outputWeight)
{
}
void ABS_FM::testmy(Sampler *sampler, saidx64_t *SA, bool originweightmethod, uint *outputWeight)
{}
struct compare
{
	bool operator()(const pair<int, int> &value,
					const int &key)
	{
		return (value.second < key);
	}
	bool operator()(const int &key,
					const pair<int, int> &value)
	{
		return (key < value.second);
	}
};


vector<string> ABS_FM::AllPossiblePattern(string str, int len)
{
	int strLen = str.length();
	// int totalPossiblePattern = (((int)pow(strLen, len + 1)) - strLen) / (strLen - 1);
	i64 totalPossiblePattern = GetDenbihe(strLen, len);
	// vector<char> charInstr(begin(str.c_str()),end(str.c_str()));
	vector<string>
		Result;
	Result.resize(totalPossiblePattern);

	int lastpos = 0;
	for (int i = 0; i < strLen; i++)
	{
		// char ch = str[i];
		Result[lastpos++] = str.substr(i, 1);
	}

	int curLen = strLen;
	for (int i = 1; i < len; i++)
	{
		auto curpos = lastpos - curLen;
		for (int j = 0; j < curLen; j++)
		{
			string curpattern = Result[curpos];
			for (int z = 0; z < strLen; z++)
			{
				string newPattern = curpattern + str[z];
				// cout<<newPattern<<endl;
				Result[lastpos++] = newPattern;
			}
			++curpos;
		}
		curLen = curLen * strLen;
	}
	// charInstr.resize(strlen);
	// charInstr(str.c_str());
	return Result;
}
i64 ABS_FM::SizeInByte_locate()
{
	return TreeSizeInByte(root);
	//+ SAL->GetMemorySize();
}
vector<i64> ABS_FM::getencoderadio()
{
	vector<i64> encode;
	encode.push_back(getrlgradio(root)+getefradio(root)+getallradio(root)+getplainradio(root));
	encode.push_back(getrlgradio(root));
	encode.push_back(getplainradio(root));
	encode.push_back(getallradio(root));
	encode.push_back(getefradio(root));
	return encode;
}
i64 ABS_FM::getrlgradio(BitMap *r)
{
	u64 num=0;
	if(r->Left()){
		num+=getrlgradio(r->Left());
	}
	if(r->Right()){
		num+=getrlgradio(r->Right());
	}
	num+=r->getrlgradio();
	return num;
}
i64 ABS_FM::getefradio(BitMap *r)
{
	u64 num=0;
	if(r->Left()){
		num+=getefradio(r->Left());
	}
	if(r->Right()){
		num+=getefradio(r->Right());
	}
	num+=r->getefradio();
	return num;
}
i64 ABS_FM::getallradio(BitMap *r)
{
	u64 num=0;
	if(r->Left()){
		num+=getallradio(r->Left());
	}
	if(r->Right()){
		num+=getallradio(r->Right());
	}
	num+=r->getallradio();
	return num;
}
i64 ABS_FM::getplainradio(BitMap *r)
{
	u64 num=0;
	if(r->Left()){
		num+=getplainradio(r->Left());
	}
	if(r->Right()){
		num+=getplainradio(r->Right());
	}
	num+=r->getplainradio();
	return num;
}
i64 ABS_FM::SizeInByte()
{
	// cout<<__FILE__<<":"<<__LINE__<<endl;
	i64 SALSize = 0, RANKLSize = 0, posrootsize = 0, hashSAL = 0, hashtable = 0, newDsize = 0,newInEntersize = 0;
	// cout << "treesizeofroot:" << TreeSizeInByte(root)<<endl;
	if (SAL != 0x0)
	{
		SALSize = SAL->GetMemorySize();
		// cout << "SALSize:" << SALSize << endl;
		// cout <<"salofsize:" << SALSize << endl;
	}
	if (RankL != 0x0)
	{
		RANKLSize = RankL->GetMemorySize();
		// cout << "RANKLSize:" << RANKLSize << endl;
		// cout <<"ranklofsize:" << RankL->GetMemorySize() << endl;
	}
	if (newD != 0x0)
	{
		newDsize = newD->GetMemorySize();

	}
	if(newInEnter != 0x0)
	{
		newInEntersize = newInEnter->GetMemorySize();
	}
//cout<<"newD:	"<<newDsize<<endl<<"newInEnter:		"<<newInEntersize<<endl;
	return TreeSizeInByte(root) + SALSize + RANKLSize + posrootsize + hashSAL + hashtable + newDsize + newInEntersize;
}

i64 ABS_FM::SizeInByte_extract()
{
	return TreeSizeInByte(root);
	// + RankL->GetMemorySize();
}

void ABS_FM::Codedistribution(int &Plain, int &AL, int &Other)
{
	Plain = BitMap::Plaincount;
	Other = BitMap::Othercount;
	AL = BitMap::ALLcount;
}

BitMap *ABS_FM::GetRoot()
{
	return root;
}
i64 ABS_FM::SizeInByte_count()
{
	return TreeSizeInByte(root);
}
i64 ABS_FM::SizeInBytePart_count(string str)
{
	return SizeOfpart(root, str);
}

map<i64, i64> ABS_FM::getBitMapRuns()
{
	return BitMap::mapruns;
	//return MergeBitMapRuns(root);
}
map<i64, i64> ABS_FM::getBitnodeRuns()
{
	return BitMap::noteruns;
	//return MergeBitnodeRuns(root);
}
int ABS_FM::TreeNodeCount(BitMap *r)
{
	if (r == NULL)
		return 0;
	return TreeNodeCount(r->Left()) + TreeNodeCount(r->Right()) + 1;
}

i64 ABS_FM::TreeSizeInByte(BitMap *r)
{
	i64 size = 0;
	if (r == NULL)
		return 0;
	if (r->Left())
		size += TreeSizeInByte(r->Left());
	if (r->Right())
		size += TreeSizeInByte(r->Right());
	size = size + r->SizeInByte();
	return size;
}
//test
i64 ABS_FM::SizeOfpart(BitMap *r, string str)
{
	int size = 0;
	if (r->Left())
		size += SizeOfpart(r->Left(), str);
	if (r->Right())
		size += SizeOfpart(r->Right(), str);
	//size = size + r->getsize();
	if (str == "SB")
		size = size + r->SizeInSuperblock();
	else if (str == "B")
		size = size + r->SizeInblock();
	else if (str == "style")
		size = size + r->SizeIncodesytle();
	else if (str == "code")
		size = size + r->SizeInMemory();
}
void MapMerge(map<i64, i64> &map1, map<i64, i64> map2)
{
	for (map<i64, i64>::iterator it = map2.begin(); it != map2.end(); it++)
	{
		if (map1.find(it->first) != map1.end())
		{
			map1[it->first] += it->second;
		}
		else
		{
			map1.insert(pair<i64, i64>(it->first, it->second));
		}
	}
}
map<i64, i64> ABS_FM::MergeBitMapRuns(BitMap *r)
{
	map<i64, i64> mapresult;
	if (r->Left())
		MapMerge(mapresult, MergeBitMapRuns(r->Left()));
	if (r->Right())
		MapMerge(mapresult, MergeBitMapRuns(r->Right()));
	MapMerge(mapresult, r->mapruns);
	return mapresult;
}
map<i64, i64> ABS_FM::MergeBitnodeRuns(BitMap *r)
{
	map<i64, i64> mapresult;
	if (r->Left())
		MapMerge(mapresult, MergeBitMapRuns(r->Left()));
	if (r->Right())
		MapMerge(mapresult, MergeBitMapRuns(r->Right()));
	MapMerge(mapresult, r->noteruns);
	return mapresult;
}
//test
int flag3 = 0;
// voi ABS_FM::DrawBackSearch()
// {
// 	for (int j = L; j <= R; j++)
// 	{
// 		f = 0;
// 		i = j;
// 		step = 0;
// 		while (newD->getD(i) != 1)
// 		{								 //D数组的D[i]
// 			i = this->Phi0->GetValue(i); //得下一个后缀位置
// 			step++;
// 			if (L <= i && i <= R)
// 			{ //如果i在[L,R]之间
// 				distance[j - L] = step;
// 				pred[i - L] = j;
// 				f = 1;
// 				break;
// 			}
// 		}
// 		if (f == 0)
// 		{
// 			i = newD->rank(i);
// 			pos[j - L] = SALD->GetValue(i - 1) * D - step;
// 			if (pos[j - L] < 0)
// 				pos[j - L] = pos[j - L] + n;
// 		}
// 	}
// 	int fu = 0;
// 	for (int j = L; j <= R; j++)
// 	{
// 		if (pos[j - L] != 0)
// 		{
// 			q = j;
// 			while (pred[q - L] != -1)
// 			{
// 				fu++;
// 				s = pos[q - L];
// 				i = pred[q - L];
// 				step = distance[i - L];
// 				pos[i - L] = s - step;
// 				pred[q - L] = -1;
// 				q = i;
// 			}
// 		}
// 	}
// }
void ABS_FM::DrawBackSearch(const char *pattern, i64 &Left, i64 &Right)//origin drawback 0512
{
	//if
	int len = 0;
	len = strlen(pattern);
	i64 occ_left = 0;
	i64 occ_right = 0;
	if (len <= 0)
	{
		Left = 1;
		Right = 0;
		return;
	}
	int i = len - 1;
	unsigned char c = pattern[i];
	int coding = code[c];
	//uchar被定义为-1 结果是255 这里判断会错
	//if (coding==-1)
	//wei
	if (coding == -1)
	{
		Left = 1;
		Right = 0;
		return;
	}
	//wei
	Left = C[coding];
	Right = C[coding + 1] - 1;
	//cout<<"Left="<<setw(10)<<Left<<","<<"Right="<<setw(10)<<Right<<","<<Right-Left<<endl;//flag
	i = i - 1;
	while ((Left <= Right) and (i >= 0))
	{
		c = pattern[i];
		coding = code[c];
		//uchar被定义为-1 结果是255 这里判断会错
		//if (coding==-1)
		if (coding == -1)
		{
			Left = 1;
			Right = 0;
			return;
		}
		Occ(c, Left - 1, Right, occ_left, occ_right);
		// LINE;
		// cout<<flag3++<<".occ_left:"<<setw(10)<<occ_left<<",occ_right"<<setw(10)<<occ_right<<",value:"<<occ_right-occ_left<<endl;
		Left = C[coding] + occ_left;
		Right = C[coding] + occ_right - 1;
		i = i - 1;
	}
	if (Right < Left)
	{
		Left = 1;
		Right = 0;
		return;
	}
	return;
}

void ABS_FM::Counting(const char *pattern, i64 &num)
{
	i64 Left = 1;
	i64 Right = 0;
	DrawBackSearch(pattern, Left, Right); //count
										  //	cout<<"Counting "<<Left<<" "<<Right<<endl;
	if (Right < Left)
		num = 0;
	else
		num = Right - Left + 1;
}

/*liuxi*/
unsigned char *ABS_FM::Extracting(i64 pos, i64 len)
{
	if(pos + len > n-1 || pos <0)
	{
		cout<<pos<<"  "<<len<<endl;
		cout<<pos+len<<" "<<n-1<<" "<<pos<<endl;
		cout<<"ABS_FM::Extracting  error parmaters"<<endl;
		return NULL;
	}

	unsigned char * sequence=new unsigned char[len+1];
	sequence[len]='\0';
	fm_int end = pos + len;
	fm_int anchor = 0;
	int overloop = 0;
	int step = this->D;
	overloop = end%step;
	anchor = end/step;
	if(overloop!=0){
		anchor++;
		if(anchor!=RankL->GetNum()-1){
			overloop=step-overloop;
		}else{
			overloop=(n-1)%step-overloop;
		}
	}
	fm_int i= RankL->GetValue(anchor);
	i=newD->select(i);
	for(int j=0;j<overloop;j++)
		i = LF(i);

	for(int j=0;j<len;j++)
	{
		sequence[len-1-j]=L(i);
		i = LF(i);
	}
	return sequence;
}
i64 *ABS_FM::Locating_pos(const char *pattern, i64 &num)
{
	cout<<"qqqqqq"<<endl;
	i64 Left = 1;
	i64 Right = 0;
#ifdef PROC
	cout << "DrawBackSearch start;"
		 << "Left:" << Left
		 << "Right:" << Right << endl;
#endif
	DrawBackSearch(pattern, Left, Right);
#ifdef PROC
	cout << "DrawBackSearch end;"
		 << "Left:" << Left
		 << "Right:" << Right << endl;
#endif
	//
	if (Right < Left)
	{
		num = 0;
		return NULL;
	}
	num = Right - Left + 1;
	Locatetimes += num;
	i64 *pos = new i64[num];
#ifdef PROC
	cout << "Lookup loop start ;"
		 << "Left:" << Left
		 << "Right:" << Right << endl;
#endif
	for (int i = 0; i < num; i++)
		//pos[i]=Lookup(Left + i);
		// if (oldsample)
			pos[i] = Lookup(Left + i);
		// else
		// {
		// 	// pos[i] = Lookupfornew(Left + i);
		// 	pos[i] = Lookupforhash(Left + i);
		// }
#ifdef PROC
	cout << "Lookup loop end ;"
		 << "Left:" << Left
		 << "Right:" << Right << endl;
#endif
	return pos;
}
void ABS_FM::Getpos(fm_int left,fm_int right,fm_int *&pos){
	fm_int num=right-left+1;
	fm_int *dis=new fm_int[num];//距离数组，标记当前查询的位置与上一次查询到当前位置的跳步次数
	fm_int *pred=new fm_int[num];//记录跳步到当前查询位置的上一个位置
	for(fm_int i=0;i<num;i++){
		pred[i]=-1;
		pos[i]=-1;
	}
	fm_int f=0;
	fm_int i=0;
	fm_int step=0;
	fm_int q=0;
	fm_int s=0;
	for (fm_int j = right; j >= left; j--){
		f = 0;
		i = j;
		step = 0;
		// cout<<"start####"<<j<<endl;
		while (newD->getD(i)!=1){
			i = LF(i);
			step++;
			//当跳步达到的位置在左右边界内，则记录跳步次数，及再次跳步到查询区间的查询位置
			if (left <= i&&i <= right){
				dis[j - left] = step;
				pred[i - left] = j;
				f = 1;
				break;
			}
			
		}
		//标志位为0，表示查询位置跳步到采样殿过程中没有再次进入查询区间，由采样点和跳步次数获取出现位置
		if (f == 0){
			fm_int r=newD->rank(i);
			fm_int sal=0;
			//若当前文本长度不为采样步长的整数倍，则表示在采样时，标记数组对于sa[0]=n-1的位置多记录了一次，因此要r-2,否则r-1
			if((n-1)%D==0){
				sal=SAL->GetValue(r-1);
			}else{
				sal=SAL->GetValue(r-2);
			}
			pos[j - left] = (sal*D+step)%n;
		}
	}
	for(fm_int j=left;j<=right;j++){
		if(pos[j-left]!=-1){
			q=j;
			while(pred[q-left]!=-1){ 
				s=pos[q-left];
				i=pred[q-left];
				step=dis[i-left];
				pos[i-left]=s+step;
				pred[q-left]=-1;
				q=i;
			}
		}
	}
	delete [] pred;
	delete [] dis;
}
i64 *ABS_FM::Locating_value(const char *pattern, i64 &num)
{
	fm_int Left=1;
	fm_int Right = 0;
	DrawBackSearch(pattern,Left,Right);
	if(Right < Left)
		return NULL;
	num = Right - Left + 1;
	fm_int *pos =new fm_int[num];
	Getpos(Left,Right,pos);
	return pos;
}

i64 *ABS_FM::Locating2(const char *pattern, i64 &num)
{
	i64 Left = 1;
	i64 Right = 0;
#ifdef PROC
	cout << "DrawBackSearch start;"
		 << "Left:" << Left
		 << "Right:" << Right << endl;
#endif
	DrawBackSearch(pattern, Left, Right);
#ifdef PROC
	cout << "DrawBackSearch end;"
		 << "Left:" << Left
		 << "Right:" << Right << endl;
#endif
	//
	if (Right < Left)
	{
		num = 0;
		return NULL;
	}
	num = Right - Left + 1;
	Locatetimes += num;
	i64 *pos = new i64[num];
#ifdef PROC
	cout << "Lookup loop start ;"
		 << "Left:" << Left
		 << "Right:" << Right << endl;
#endif
	for (int i = 0; i < num; i++)
		//pos[i]=Lookup(Left + i);
		if (oldsample)
			pos[i] = Lookup(Left + i);
		else
		{
			pos[i] = Lookupfornew(Left + i);
			// pos[i] = Lookupforhash(Left + i);
		}
#ifdef PROC
	cout << "Lookup loop end ;"
		 << "Left:" << Left
		 << "Right:" << Right << endl;
#endif
	return pos;
}

	int flag = 0;

i64 ABS_FM::Lookup(i64 i)
{
	//cout<<i<<"++"<<endl;
	//if(v_hittimes.size()==0){
	//	v_hittimes.insert(v_hittimes.begin(),n,0);
	//	}
	int ori_i = i;
	//cout<<"i:"<<i;
	int step = 0;
	int D = this->D;
	while (i % D != 0)
	{
		//	v_hittimes[i]++;
		i = LF(i);
		step = step + 1;
#ifdef PROC
		if (step > 500)
			exit(0);
		cout << ",i:" << i << setw(10) << ":step:" << step << setw(10) << endl;
#endif
	}
	i = i / D;
	return (SAL->GetValue(i) + step) % n;
}
i64 ABS_FM::Lookupfornew(i64 i)
{
	int step = 0;
	//int D = this->D*16;
	int flag = 0;
	i64 rank = 0;
	while (newD->getD(i) != 1)
	{
		//v_hittimes[i]++;
		i = LF(i);
		step = step + 1;

	}
	rank = newD->rank(i);
	return (SAL->GetValue(rank - 1) * D + step) % n;
}


int flag2 = 0;
//返回L串中c字符在位置pos_left 和pos_right之前出现的次数，结果由rank_left 和rank_right带回.
void ABS_FM::Occ(unsigned char c, i64 pos_left, i64 pos_right, i64 &rank_left, i64 &rank_right)
{
	BitMap *r = root;
	int level = 0;
	char code = '0';
	while (r->Left())
	{
		code = codeTable[c][level];
		if (code == '1') //编码是1,走右分支
		{
			//wch
			if (pos_left > -1 && pos_right > -1 && (pos_left < pos_right)) //left right 都有待查找
			//if(pos_left>-1 && pos_right >-1) //left right 都有待查找
			//wch
			{
#ifdef PROC
				if (pos_left == 1693813)
					FILEANDLINE;
				cout << "before rank:" << endl
					 << "pos_left:" << pos_left
					 << ",pos_right:" << pos_right
					 << ",pos_right:"
					 << rank_left << ",rank_right:" << rank_right << endl;
#endif
				r->Rank(pos_left, pos_right, rank_left, rank_right);
#ifdef PROC
				cout << "after rank:" << endl;
				cout << flag2++ << ".char=" << setw(10) << c << ";rank_right=" << setw(10) << rank_right << ";rank_left=" << setw(10) << rank_left << endl;
				cout << "right" << setw(10);
				cout << __FILE__ << ":" << __LINE__ << ":" << pos_left << ";" << pos_right << ";" << rank_left << ";" << rank_right << endl;
#endif
				pos_left = rank_left - 1;
				pos_right = rank_right - 1;

				/*	pos_left = r->Rank(pos_left)-1;
				pos_right=r->Rank(pos_right)-1;
			*/
			}
			else if (pos_right > -1) //只查右分支
			{
#ifdef PROC
				cout << pos_right << ";";
				cout << "pos_right" << pos_right << endl;
#endif
				pos_right = r->Rank(pos_right) - 1;
#ifdef PROC
				cout << "after rank:" << endl;
				cout << flag2++ << ".char=" << setw(10) << c << ";rank_right=" << setw(10) << rank_right << ";rank_left=" << setw(10) << rank_left << endl;
				cout << "right" << setw(10);
				cout << __FILE__ << ":" << __LINE__ << ":" << pos_left << ";" << pos_right << ";" << rank_left << ";" << rank_right << endl;
#endif
			}
			else //该跳出循环了,此时pos_left 和pos_right都是-1.
			{
				break;
			}
			r = r->Right();
		}
		else //编码是0,走左分支.
		{
			if (pos_left > -1 && pos_right > -1 && (pos_left < pos_right))
			// if(pos_left>-1 && pos_right >-1)
			{
#ifdef PROC
				cout << "before rank:" << pos_left << "," << pos_right << "," << rank_left << "," << rank_right << endl;
#endif
				if (flag2 == 620)
					FILEANDLINE;
				r->Rank(pos_left, pos_right, rank_left, rank_right);
#ifdef PROC
				cout << flag2++ << ".char=" << setw(10) << c << ";rank_right=" << setw(10) << rank_right << ";rank_left=" << setw(10) << rank_left << endl;
				cout << "left" << setw(10);
				cout << pos_left << ";" << pos_right << ";" << rank_left << ";" << rank_right << endl;
#endif
				pos_left = (pos_left + 1) - rank_left - 1;
				pos_right = (pos_right + 1) - rank_right - 1;

				/*
				pos_left = (pos_left+1)-r->Rank(pos_left)-1;
				pos_right= (pos_right+1)-r->Rank(pos_right)-1;
			*/
			}
			else if (pos_right > -1)
			{
// cout<<pos_right<<";";
// if(pos_right==353008)
// LINE;
#ifdef PROC
				cout << "pos_right" << pos_right << endl;
#endif
				pos_right = (pos_right + 1) - r->Rank(pos_right) - 1;
#ifdef PROC
				cout << flag2++ << "pos_right=" << setw(10) << pos_right << endl;
				cout << __LINE__ << ":" << pos_right << endl;
#endif
			}
			else
			{
				break;
			}
			r = r->Left();
		}
		level++;
	}
	rank_left = pos_left + 1;
	rank_right = pos_right + 1;
	return;
}

i64 ABS_FM::Occ(unsigned char c, i64 pos)
{
	BitMap *r = root;
	int level = 0;
	char code = '0';
	while (r->Left() && pos > -1)
	{
		code = codeTable[c][level];
		if (code == '1')
		{
			pos = r->Rank(pos) - 1;
			r = r->Right();
		}
		else
		{
			pos = (pos + 1) - r->Rank(pos) - 1;
			r = r->Left();
		}
		level = level + 1;
	}
	return pos + 1;
}


i64 ABS_FM::LF(i64 i)
{
	
#ifdef COUNTLFTIMES
	LFtimes++;
#endif
	i64 occ = 0;
	unsigned char label = 0;
	Occ(occ, label, i);
	int coding = code[label];
	return occ + C[coding] - 1;
}

unsigned char ABS_FM::L(i64 i)
{
	BitMap *r = root;
	int bit = 0;
	i64 rank = 0;

	while (r->Left())
	{
		//	bit = r->Rank(i) - r->Rank(i-1);
		rank = r->Rank(i, bit);
		if (bit == 1)
		{
			//i = r->Rank(i)-1;
			i = rank - 1;
			r = r->Right();
		}
		else
		{
			//i = (i+1) - r->Rank(i)-1;
			i = (i + 1) - rank - 1;
			r = r->Left();
		}
	}
	return r->Label();
}
int flag4 = 0;
i64 ABS_FM::Occ(i64 &occ, unsigned char &label, i64 pos)
{
	BitMap *r = root;
	//cout<<"occ:root"<<endl;
	int bit = 0;
	i64 rank = 0;
	while (r->Left())
	{
		

		rank = r->Rank(pos, bit);
#ifdef PROC
		cout << setw(20) << flag4++ << "  pos=" << setw(10) << pos << ",Bit = " << bit << ",rank=" << setw(10) << rank << endl;
#endif
		if (bit == 1)
		{
			//cout<<"r"<<endl;
			pos = rank - 1;
			r = r->Right();
		}
		else
		{
			//cout<<"l"<<endl;
			pos = (pos + 1) - rank - 1;
			r = r->Left();
		}
	}
	occ = pos + 1;
	label = r->Label();
	return 0;
}
extern int length;
//read filedate into T and cal Cumulative Frequency sum(C)
unsigned char *ABS_FM::Getfile(const char *filename)
{
	this->filename = filename;
	FILE *fp = fopen(filename, "r+");
	if (fp == NULL)
	{
		cout << "Be sure the file is available" << endl;
		exit(0);
	}
	fseeko(fp, 0, SEEK_END);
	n = ftello(fp) + 1;
	unsigned char *T = new unsigned char[n];
	fseeko(fp, 0, SEEK_SET);
	int e = 0;
	int num = 0;
	num = fread(T, sizeof(uchar), n, fp);
	T[n - 1] = 0;
	fclose(fp);
	memset(charFreq, 0, 256 * sizeof(i64));
	memset(charMap, 0, 256 * sizeof(bool));
	for (saidx64_t i = 0; i < n; i++)
		charFreq[T[i]]++;
	this->alphabetsize = 0;
	for (i32 i = 0; i < 256; i++)
		if (charFreq[i])
		{
			this->alphabetsize++;
			this->charMap[i] = true;
		}
	this->code = new short[256];
	this->C = new i64[alphabetsize + 1];
	memset(C, 0, (alphabetsize + 1) * 4);
	this->C[alphabetsize] = n;
	this->C[0] = 0;
	int k = 1;
	i64 pre = 0;
	for (int i = 0; i < 256; i++)
	{
		if (charFreq[i] != 0)
		{
			code[i] = k - 1;
			C[k] = pre + charFreq[i];
			pre = C[k];
			k++;
		}
		else
			code[i] = -1;
	}
	return T;
}
int ABS_FM::BWT(unsigned char *T, int *SA, unsigned char *bwt, int len)
{
	int i = 0;
	int index = 0;
	for (i = 0; i < len; i++)
	{
		index = (SA[i] - 1 + len) % len;
		bwt[i] = T[index];
	}
	return 0;
}
//WCH BWT64
int ABS_FM::BWT64(unsigned char *T, saidx64_t *SA, unsigned char *bwt, saidx64_t len)
{
	saidx64_t i = 0;
	saidx64_t index = 0;
	for (i = 0; i < len; i++)
	{
		index = (SA[i] - 1 + len) % len;
		bwt[i] = T[index];
		if (index == len - 1)
		{
			bwtIndex = i;
		}
	}
	return 0;
}
i64 ABS_FM::GetEnterPosRank(i64 i)
{
	// root->Rank(i,'\n');

	// posenter->rank(i,flag);
	int flag = 0;
	// FILEANDLINE;
	// cout << posenter->Rank(i, flag)<<endl ;
	return newInEnter->rank2(i);
}
void ABS_FM::SASample(saidx64_t *SA)
{
	int step1 = this->D;
	int datewidth = log2(n) + 1;
	SAL = new InArray(n / step1 + 1, fmablog(n/step1)+1);
	SALPos = new InArray(n / step1 + 1, 1);
	newD = new Dmap(n, 1);
	i64 i = 0;
	i64 j = 0;
	for(i=0;i<n;i++){
		if(SA[i]%step1==0){
			SAL->SetValue(j++,SA[i]/step1);
			newD->SetValue(i,1);
		}else if(i==0){
			newD->SetValue(i,1);
		}else{
			newD->SetValue(i,0);
		}
	}
	newD->constructrank(256,16);
	if((n-1)%step1!=0){
		RankL->SetValue((n-1)/step1+1,newD->rank(0));
	}
	for(i=0;i<n;i++)
	{
		if(SA[i]%step1==0){
			RankL->SetValue(SA[i]/step1,newD->rank(i));
		}
	}
}
void ABS_FM::SASample_enter(saidx64_t *SA)
{
	int step1 = this->D;
	//int step2 =this->D*16;
	// int datewidth = log2(n/step1)+1;
	int datewidth = log2(n) + 1;
	SAL = new InArray(n / step1 + 1, datewidth); //SA sample
	SALPos = new InArray(n / step1 + 1, 1);
	unsigned char *pos = new unsigned char[n];
	//for(i=0,j=0;i<n;i=i+step1,j++)
	//	SAL->SetValue(j,SA[i]);
	int j = 0;
	//cout<<"SASample start"<<endl;
	// newD = new Dmap(n, 1);
	uchar Creal[256] = {0};
	int count = 0;
	for (int i = 0; i < 256; i++)
	{
		if (charMap[i])
			Creal[i] = count++;
	}
	saidx64_t start = 0;
	saidx64_t end = 0;
	start = C[Creal['\n']];
	end = C[Creal['\n'] + 1];
	for (int i = 0; i < n; i++)
	{
		if (SA[i] % step1 == 0)
		{
			SAL->SetValue(j++, SA[i] / step1);
			pos[i] = 1;
			newD->SetValue(i, 1);
			//cout<<i<<endl;
		}
		else
		{
			newD->SetValue(i, 0);
			pos[i] = 0;
		}
	}
	newD->constructrank(256, 16);
}
int ABS_FM::BuildTree(int speedlevel, int method, bool calsample)
{
	if(calsample)
	{
		sa = new saidx64_t[n];
		divsufsort64(T, sa, n);
		//SA和Rank数组的采样
		int step1 = this->D;
		int step2 = this->D;
		int datewidth = fmablog(n);

		bwt = new unsigned char[n];
		BWT64(T, sa, bwt, n);
		double runs = 0.0;
		for (i64 i = 0; i < n - 1; i++)
			if (bwt[i] != bwt[i + 1])
				runs++;
		runs = n / runs;
		this->total_runs = runs;
		TreeCode();
		root = CreateWaveletTree(bwt, n, codeTable,method);
		RankL = NULL;
		SAL = NULL;
		newD = NULL;
		InEnter = NULL;
		newInEnter = NULL;
		i64 i = 0;
		i64 j = 0;
		newD = NULL;
		RankL = new InArray(n / step2 + 2, fmablog(n/step1)+1); //rank sample
		oldsample = false;
		SASample(sa);
	}
	else
	{
		saidx64_t *SA = new saidx64_t[n];
		divsufsort64(T, SA, n);
		//SA和Rank数组的采样
		int step1 = this->D;
		int step2 = this->D;
		int datewidth = fmablog(n);

		bwt = new unsigned char[n];
		BWT64(T, SA, bwt, n);
		double runs = 0.0;
		for (i64 i = 0; i < n - 1; i++)
			if (bwt[i] != bwt[i + 1])
				runs++;
		runs = n / runs;
		this->total_runs = runs;
		TreeCode();
		root = CreateWaveletTree(bwt, n, codeTable,method);
		RankL = NULL;
		SAL = NULL;
		newD = NULL;
		InEnter = NULL;
	}
	return 0;
}

//test
void ABS_FM::getbwtRuns(unsigned char *bwt, int len)
{
	unsigned char ch = bwt[0];
	i64 runs = 1;
	for (int i = 1; i < len; i++)
	{
		if (bwt[i] == ch)
			runs++;
		else
		{
			map<i64, i64>::iterator it = BWTruns.find(runs);
			if (it == BWTruns.end())
			{
				BWTruns.insert(pair<i64, i64>(runs, 1));
			}
			else
			{
				BWTruns[runs]++;
			}
			ch = bwt[i];
			runs = 1;
		}
	}
}
void ABS_FM::Test_Shape(BitMap *r)
{
	if (r->Left() && r->Right())
	{
		Test_Shape(r->Left());
		Test_Shape(r->Right());
	}
	else if (r->Left() || r->Right())
	{
		cout << "one child" << endl;
	}
}

void ABS_FM::Test_Occ()
{
	int count[256] = {0};
	unsigned long long int mis = 0;
	for (int i = 0; i < n; i++)
	{
		count[bwt[i]]++;
		if (Occ(bwt[i], i) != count[bwt[i]])
		{
			cout << count[bwt[i]] << " " << Occ(bwt[i], i) << " " << (int)bwt[i] << " " << (char)bwt[i] << " " << i << endl;
			cout << codeTable[bwt[i]] << endl;
			mis++;
		}
		if (i % 100000 == 0)
			cout << (i * 1.0) / n << endl;
	}
	cout << "missing :" << mis << endl;
}

void ABS_FM::Test_L()
{
	int mis = 0;
	for (int i = 0; i < n; i++)
	{
		if (bwt[i] != L(i))
		{
			cout << bwt[i] << " " << L(i) << " " << i << endl;
			mis++;
			;
		}
	}
	cout << "mis: " << mis << endl;
}

BitMap *ABS_FM::CreateWaveletTree(unsigned char *bwt, i64 n, char ctable[CHAR_SET_SIZE][CHAR_SET_SIZE],int method)
{
	BitMap *root = NULL;

	root = FullFillWTNode(bwt, n, 0, ctable,method);
	if (!root)
	{
		cout << "FullfillWTNode failed" << endl;
		exit(0);
	}
	return root;
}
BitMap *ABS_FM::FullFillWTNode(unsigned char *buff, i64 len, int level, char ctable[CHAR_SET_SIZE][CHAR_SET_SIZE],int method)
{
	int CurrentLevel = level;
	i64 CurrentBitLen = len;
	unsigned char CurrentLabel = '\0';
	unsigned long long int *CurrentBitBuff = NULL;
	if ((int)strlen((const char *)ctable[buff[0]]) == level)
	{
		CurrentLabel = buff[0];
		CurrentBitBuff = NULL;
		uchar *tables[4] = {this->Z, this->R, this->R1, this->R3};
		BitMap *node = new BitMap(CurrentBitBuff, CurrentBitLen, CurrentLevel,method, block_size, CurrentLabel, tables);
		node->Left(NULL);
		node->Right(NULL);
		return node;
	}

	i64 u64Len = 0;
	if (len % 64 == 0)
		u64Len = len / 64 + 1;
	else
		u64Len = len / 64 + 2;
	CurrentBitBuff = new unsigned long long int[u64Len];
	memset(CurrentBitBuff, 0, u64Len * 8);
	unsigned char *lptr = NULL;
	unsigned char *rptr = NULL;
	i64 leftLen = 0;
	i64 rightLen = 0;

	lptr = new unsigned char[len];
	rptr = new unsigned char[len];
	memset(lptr, 0, len);
	memset(rptr, 0, len);

	i64 i = 0;
	int bytePos = 0;
	int bitOffset = 0;
	u64 last = 0;
	for (i = 0; i < len; i++)
	{
		if (ctable[buff[i]][level] == '1') //judge go left or right
		{
			CurrentBitBuff[bytePos] |= (0x01ull << (63 - bitOffset));
			rptr[rightLen++] = buff[i];
			last = 0;
		}
		else
		{
			lptr[leftLen++] = buff[i];
			last = 1;
		}
		bitOffset++;
		if (bitOffset == 64)
		{
			bytePos++;
			bitOffset = 0;
		}
	}
	CurrentBitBuff[bytePos] |= (last << (63 - bitOffset));

	uchar *tables[4] = {this->Z, this->R, this->R1, this->R3};
	BitMap *node = new BitMap(CurrentBitBuff, CurrentBitLen, CurrentLevel, method,block_size, CurrentLabel, tables);
	if (leftLen != 0)
	{
		BitMap *left = FullFillWTNode(lptr, leftLen, level + 1, ctable,method);
		node->Left(left);
		delete[] lptr;
		lptr = NULL;
	}
	if (rightLen != 0)
	{
		BitMap *right = FullFillWTNode(rptr, rightLen, level + 1, ctable,method);
		node->Right(right);
		delete[] rptr;
		rptr = NULL;
	}
	return node;
}

int ABS_FM::DestroyWaveletTree()
{
	delete root;
	root = NULL;
	return 0;
}

int ABS_FM::blog(int x)
{
	int ans = 0;
	while (x > 0)
	{
		ans++;
		x = (x >> 1);
	}
	return ans;
}
void ABS_FM::buildEFtable(u64 i){
	int decodelen = 0;
	u16 x = i;
	while(1){
		if(x&1) break;
		x = x>>1;
		decodelen++;
	}
	R3[i*18] = 16-decodelen;

	int onesnum = popcnt1(i);
	R1[i] = R3[i*18]-onesnum;
	//cout<<(int)R1[i]<<" "<<(int)R2[i]<<" "<<(int)R3[i*18]<<endl;

	x = i;
	int zeronums = 0;
	int alreadyBits = 0;
	int index = 0;
	while(alreadyBits < R3[i*18]){
		int num_0 = Zeros(x,this);
		zeronums += num_0;
		alreadyBits += num_0;
		x = x<<num_0;
		int num_1 = Zeros(~x,this);
		R3[i*18+zeronums+1] = num_1;//桶中元素个数
		//[i*35+18+zeronums] = alreadyBits;//桶的起始位置
		alreadyBits += num_1;
		x = x<<num_1;
	}
}
//init tables
void ABS_FM::Inittable()
{
	this->Z = new uchar[1 << 8];
	int tablewidth = 8;
	for (int i = 0; i < tablewidth; i++)
		for (int j = (1 << i); j < (2 << i); j++)
			Z[j] = tablewidth - 1 - i;
	Z[0] = tablewidth;

	u64 tablesize = (1 << 16);
	R = new uchar[tablesize << 2];
	R1 = new uchar[tablesize]();
	R3 = new uchar[tablesize*18]();
	//查找表的初始化：在16bits的0,1串上模拟gamma解码的过程，得到
	//这些表
	u64 B[2] = {0xffffffffffffffffull, 0xffffffffffffffffull};
	int sum = 0;  //gamma编码的和,含义为原串被编码的bits数目。
	i64 step = 0; //16bits可以完整解码的bits数,该值不会大于16.
	int rank = 0; //16bits表示的几个完整的runs,假设第一个runs是1-runs,这几个runs的rank值。
	int runs = 0; //runs 个数.

	int x = 0;		 //工作变量，保存本次的gamma解码值.
	int prestep = 0; //前一次正确解码的bits数(累加),<=16.
	for (u64 i = 0; i < tablesize; i++)
	{
		B[0] = (i << 48);
		sum = 0;
		step = 0;
		prestep = 0;
		rank = 0;
		runs = 0;
		while (1)
		{
			x = GammaDecode(B, step, this); //step会联动.
			if (step > 16)
				break;
			sum = sum + x;
			prestep = step;
			runs++;
			if (runs % 2 == 1)
				rank = rank + x;
		}
		R[i << 2] = runs;		   //r4
		R[(i << 2) + 1] = prestep; //r2
		R[(i << 2) + 2] = sum;	 //r1;
		R[(i << 2) + 3] = rank;	//r3

		if(i==0){
			R1[0] = 16;
			R3[0] = 16;
			continue;
		}
		buildEFtable(i);
	}
}

//递归保存节点的编号信息
int ABS_FM::SaveNodePosition(BitMap *r, u32 position, savekit &s)
{
	if (!r)
		return 1;
	s.writei32(position);
	SaveNodePosition(r->Left(), 2 * position, s);
	SaveNodePosition(r->Right(), 2 * position + 1, s);
	return 0;
}

//递归保存节点的数据信息
int ABS_FM::SaveNodeData(BitMap *r, savekit &s)
{
	if (!r)
		return 1;
	r->Save(s);
	SaveNodeData(r->Left(), s);
	SaveNodeData(r->Right(), s);
	return 0;
}

int ABS_FM::SaveWTTree(savekit &s, BitMap *r)
{
	SaveNodePosition(r, 1, s);
	SaveNodeData(r, s);
	return 0;
}

BitMap *ABS_FM::LoadWTTree(loadkit &s, int alphabetsize, uchar **tables)
{
	//读取数据，map的int域对应该节点的位置
	int nodecount = 2 * alphabetsize - 1;
	//	cout<<alphabetsize<<endl;
	//	s.loadi32(nodecount);
	int *p = new int[nodecount];
	s.loadi32array(p, nodecount);
	map<int, BitMap *> pmap;
	BitMap *r = NULL;
	for (int i = 0; i < nodecount; i++)
	{
		if (tables)
			r = new BitMap(tables);
		else
			r = new BitMap();
		r->Load(s);
		pmap[p[i]] = r;
	}
	//挂链
	map<int, BitMap *>::iterator iter;
	map<int, BitMap *>::iterator f_iter;
	for (iter = pmap.begin(); iter != pmap.end(); iter++)
	{
		f_iter = pmap.find(2 * iter->first);
		if (f_iter != pmap.end())
			iter->second->Left(f_iter->second);
		else
			iter->second->Left(NULL);

		f_iter = pmap.find(2 * iter->first + 1);
		if (f_iter != pmap.end())
			iter->second->Right(f_iter->second);
		else
			iter->second->Right(NULL);
	}
	//	cout<<"767"<<endl;
	f_iter = pmap.find(1);
	BitMap * rootcur = NULL;
	if (f_iter != pmap.end())
		rootcur = f_iter->second;
	else
	{
		rootcur = NULL;
		// exit(0);
	}
	delete[] p;
	p = NULL;
	//	cout<<"778"<<endl;
	return rootcur;
}

int ABS_FM::Load(loadkit &s)
{
	s.loadu64(this->bwtIndex);
	s.loadi64(this->n);
	s.loadi32(this->alphabetsize);
	s.loadi32(this->D);
	s.loadi32(this->block_size);
	this->C = new i64[alphabetsize + 1];
	s.loadi64array(this->C, alphabetsize + 1);

	this->code = new i16[256];
	s.loadi16array(this->code, 256);
	memset(codeTable, 0, sizeof(codeTable));
	for (int i = 0; i < 256; i++)
	{
		uchar len = 0;
		s.loadu8(len);
		if (len != 0)
		{
			charMap[i] = 1;
			int bytes = len % 8 ? len / 8 + 1 : len / 8;
			uchar *bits = new uchar[bytes];
			s.loadu8array(bits, bytes);
			int in_index = 0;
			int off_index = 0;
			for (int j = 0; j < len; j++)
			{
				if (bits[off_index] & (0x01 << (7 - in_index)))
					codeTable[i][j] = '1';
				else
					codeTable[i][j] = '0';
				in_index++;
				if (in_index == 8)
				{
					in_index = 0;
					off_index++;
				}
			}
		}
		else
			charMap[i] = 0;
	}
	Inittable();
	uchar *par[4] = {Z, R,R1,R3};
	this->root = LoadWTTree(s, alphabetsize, par);
	T = NULL;
	bwt = NULL;
	return 0;
}

int ABS_FM::LoadSAL(loadkit &s)
{
	s.loadi32(this->alphabetsize2);
	s.loadi32(this->D);
	unsigned char savemode;
	s.loadu8(savemode);
	this->SAL = new InArray();
	if(savemode == 1)
		this->SAL->load(s);
	this->RankL = new InArray();
	this->RankL->load(s);
	u8 InEnterexits = 0;
	s.loadu8(InEnterexits);
	this->newInEnter = new Dmap();
	enterposflag = false;
	if (InEnterexits == 1)
	{
		newInEnter->load(s);
		enterposflag = true;
	}
	
	u8 newdexits = 0;
	s.loadu8(newdexits);
	if (newdexits == 1)
	{
		this->newD = new Dmap();
		this->newD->load(s);
	}
	Inittable();
	return newdexits;
}
int ABS_FM::SaveSAL(savekit &s, char savemode)
{
	s.writei32(2);
	s.writei32(D); //SA的采样率
	//for SAL
	s.writeu8(savemode);
	if(savemode==1)//savemode = 1 save all
		SAL->write(s);
	//for RankL
	RankL->write(s);
	if (newInEnter != 0)
	{
		s.writeu8(1);
		newInEnter->write(s);
	}
	else
	{
		s.writeu8(0);
	}
	if (newD != 0)
	{
		s.writeu8(1);
		newD->write(s);
	}
	else
	{
		s.writeu8(0);
	}
	return 0;
}
int ABS_FM::Save(savekit &s)
{
	s.writeu64(getBWTIndex());
	s.writei64(n);
	s.writei32(alphabetsize);
	s.writei32(D); //SA的采样率
	s.writei32(block_size);
	//C表
	s.writei64array(C, alphabetsize + 1);
	s.writei16array(code, 256);
	//codeTable
	for (int i = 0; i < 256; i++)
	{
		uchar len = strlen(codeTable[i]);
		s.writeu8(len);
		if (0 != len)
		{
			int bytes = len % 8 ? len / 8 + 1 : len / 8;
			uchar *bits = new uchar[bytes];
			memset(bits, 0, bytes);
			int off_index = 0;
			int in_index = 0;
			for (int j = 0; j < len; j++)
			{
				if (codeTable[i][j] == '1')
					bits[off_index] = bits[off_index] | (0x01 << (7 - in_index));
				in_index++;
				if (8 == in_index)
				{
					in_index = 0;
					off_index++;
				}
			}
			s.writeu8array(bits, bytes);
		}
	}
	SaveWTTree(s, this->root);
	return 0;
}

#if 1
int ABS_FM::decmpWaveletTree(BitMap *root, int &t)
{
	if (!root)
		cout << "No Tree!\n";
	if (root->Left() == NULL && root->Right() == NULL)
	{
		return -1;
	}
	//test

	root->deCompress(t);

	if (root->Left())
	{
		//cout << "left\n";
		decmpWaveletTree(root->Left(), t);
	}
	if (root->Right())
	{
		//cout << "right\n";
		decmpWaveletTree(root->Right(), t);
	}
	return 0;
}
int ABS_FM ::decompressInitWaveletTree(BitMap *root)
{
	if (root->Left() == NULL && root->Right() == NULL)
		return 0;
	root->dePtr = root->unzipBuff;
	root->deOff = 0;
	if (root->Left())
		decompressInitWaveletTree(root->Left());
	if (root->Right())
		decompressInitWaveletTree(root->Right());
}

int ABS_FM ::genBwtWithWaveletTree(unsigned char *bwt)
{
	if (!root || !bwt)
	{
		return -1;
	}
	int ret = decompressInitWaveletTree(root);
	if (ret < 0)
	{
		//errProcess("decompressInitWaveletTree", ret);
		cout << "decompressInitWaveletTree error\n";
		return ret;
	}

	u64 i;
	//waveletNode_t *node;
	BitMap *node = NULL;
	//for (i = 0; i < root -> unzipLen; i++)
	for (i = 0; i < n; i++)
	{
		node = root;
		BitMap *tempPtr;
		while (node->Left() != NULL && node->Right() != NULL)
		{

			tempPtr = (*(node->dePtr) & (1ULL << (63 - node->deOff))) ? (node->Right()) : (node->Left());
			node->deOff++;
			if (node->deOff == 64)
			{
				node->deOff = 0;
				node->dePtr++;
			}
			node = tempPtr;
		}
		bwt[i] = node->Label();
	}
	return 0;
}

int ABS_FM ::writeToFile(const char *filename, unsigned char *T)
{

	FILE *fp = fopen(filename, "wb"); //get the pointer of file;
	if (fp == NULL)
	{
		cout << "can not find the file!" << endl;
		exit(0);
	}

	u64 e = 0;
	e = fwrite(T, sizeof(uchar), n, fp);

	if (e != n)
	{
		cout << "write the file failed!" << endl;
	}
	else
		cout << "write the file success!" << endl;
	fclose(fp);
	return 0;
}
int ABS_FM ::decompress_all(const char *filename)
{
	//cout << "here1---------\n";
	int t = 0;
	cout << __FILE__ << __LINE__ << endl;
	cout << filename << endl;
	decmpWaveletTree(root, t);
	bwt = new unsigned char[n];
	//cout << "here2---------\n";
	genBwtWithWaveletTree(bwt);

	//unsigned char* T = new unsigned char [n];
	T = new unsigned char[n];
	int ok = inv_bw_transform(bwt, T, n, bwtIndex);
	//cout << "here3---------\n";

	writeToFile(filename, T);
	//cout << "here4---------\n";
	delete[] bwt;
	bwt = NULL;
	//cout << "here5---------\n";

	delete[] T;
	T = NULL;
	return 1;
}
int ABS_FM::buildIndex(int SMethod)
{
	//cout << "here1---------\n";
	int t = 0;
	int step1 = D;
//	int step2 = D * 16;
	int step2 = D;
	int datewidth = log2(n) + 1;
	decmpWaveletTree(root, t);
	bwt = new unsigned char[n];
	sa = new saidx64_t[n];
	//cout << "here2---------\n";
	genBwtWithWaveletTree(bwt);

	//unsigned char* T = new unsigned char [n];
	T = new unsigned char[n];
	int ok = id_inv_sa_transform(bwt, sa, n, bwtIndex,0);
	InEnter = NULL;
	//cout << "here3---------\n";
	/////mnt
	// if(1)
	{
		i64 i = 0;
		i64 j = 0;
		// bool orginWeight = true;
		RankL = new InArray(n / step2 + 2, fmablog(n/step1)+1); //rank sample
		switch (SMethod)
		{
		case SampleMethod::ordinary:
		{
			SAL = new InArray(n / step1 + 1, datewidth); //SA sample
			cout << "oldsample" << endl;

			for (i = 0, j = 0; i < n; i = i + step1, j++)
			{
				SAL->SetValue(j, sa[i]);
			}
			oldsample = true;
			break;
		}
		case SampleMethod::equidistant:
		{

			oldsample = false;
			// cout << "regularsample" << endl;
			// SASample_enter(sa);
			// cout << "regularsample" << endl;
			for(i64 i = 0 ; i < n ;i++)
			{
				if(sa[i]>n)
					cout<<"break"<<endl;
			}
			SASample(sa);
			break;
		}
		}
		
	}
	delete[] bwt;
	delete[] sa;
	sa = NULL;
	bwt = NULL;
	//cout << "here5---------\n";

	delete[] T;
	T = NULL;
	return 1;
}

int ABS_FM::inv_sa_transform(unsigned char *bwt, saidx64_t *sa, u64 n, u64 idx,int method)
{
	//C map to real character
	uchar Creal[256] = {0};
	int count = 0;
	for (int i = 0; i < 256; i++)
	{
		if (charMap[i])
			Creal[i] = count++;
	}

	saidx64_t start = 0;
	saidx64_t end = 0;
	start = C[Creal['\n']];
	end = C[Creal['\n'] + 1];
	vector<i64> enterpos;
	enterpos.push_back(0);
	for (i64 i = start; i < end; i++)
	{
		// auto tt= sa[i];
		// if(sa[i]!=0)
		enterpos.push_back(sa[i]);
	}
	sort(enterpos.begin(), enterpos.end());
	//
	unsigned char *posSampled = new unsigned char[n];
	memset(posSampled, '0', n * sizeof(unsigned char));
	for (i64 i = 0; i < enterpos.size(); i++)
	{
		posSampled[enterpos[i]] = '1';
	}
	char ct[CHAR_SET_SIZE][CHAR_SET_SIZE];
	ct['1'][0] = '1';
	ct['0'][0] = '0';
	posenter = CreateWaveletTree(posSampled, n, ct,method);


	return 0;
}
int ABS_FM::id_inv_sa_transform(unsigned char *bwt, saidx64_t *sa, u64 n, u64 idx,int method)
{
	//C map to real character
	uchar Creal[256] = {0};
	int count = 0;
	for (int i = 0; i < 256; i++)
	{
		if (charMap[i])
			Creal[i] = count++;
	}

	i64 OCCtable[256] = {0};
	u64 *LF = new u64[n];

	for (u64 i = 0; i < n; i++)
	{
		if (bwt[i] != 0)
			LF[i] = OCCtable[bwt[i]] + C[Creal[bwt[i]]];
		else
			LF[i] = OCCtable[bwt[i]] + 0;
		OCCtable[bwt[i]]++;
	}
	u64 I = idx;
	for (u64 k = n - 1; k >= 0; k--)
	{
		I = LF[I];
		sa[I] = k;
		if (k == 0)
			break;
	}

	saidx64_t start = 0;
	saidx64_t end = 0;
	start = C[Creal['\n']];
	end = C[Creal['\n'] + 1];
	vector<i64> enterpos;
	enterpos.push_back(0);
	for (i64 i = start; i < end; i++)
	{
		// auto tt= sa[i];
		// if(sa[i]!=0)
		enterpos.push_back(sa[i]);
	}
	sort(enterpos.begin(), enterpos.end());
	//
	unsigned char *posSampled = new unsigned char[n];
	memset(posSampled, '0', n * sizeof(unsigned char));
	for (i64 i = 0; i < enterpos.size(); i++)
	{
		posSampled[enterpos[i]] = '1';
	}
	char ct[CHAR_SET_SIZE][CHAR_SET_SIZE];
	ct['1'][0] = '1';
	ct['0'][0] = '0';
	posenter = CreateWaveletTree(posSampled, n, ct,method);

	delete[] LF;
	LF = NULL;

	return 0;
}
int ABS_FM::inv_bw_transform(unsigned char *bwt, unsigned char *T, u64 n, u64 idx)
{
	//C map to real character
	uchar Creal[256] = {0};
	int count = 0;
	for (int i = 0; i < 256; i++)
	{
		if (charMap[i])
			Creal[i] = count++;
	}

	int OCCtable[256] = {0};
	u32 *LF = new u32[n];

	for (u64 i = 0; i < n; i++)
	{
		if (bwt[i] != 0)
			LF[i] = OCCtable[bwt[i]] + C[Creal[bwt[i]]];
		else
			LF[i] = OCCtable[bwt[i]] + 0;
		OCCtable[bwt[i]]++;
	}

	u64 I = idx;

	for (u64 k = n - 1; k >= 0; k--)
	{
		T[k] = bwt[I];
		I = LF[I];
		if (k == 0)
			break;
	}
	delete[] LF;
	LF = NULL;
	return 0;
}

#endif
