#include"flow.h"
#include"preMethod.h"
#include "algorithm"
extern double reads_runs;
void FTcomp::paraCMPID(std::vector<std::string>ID,int blockSize, int sample,int shape, std :: string savePath)
{
	std::cout << "Compress ID files....\n";
	cmpID(ID, blockSize,sample, shape, savePath);
	GetIDindex(savePath, savePath);
	return;
}
void FTcomp::paraCMPDNA(int seqlength,int blockSize, int shape, std :: string savePath,int dna__len)
{
	int partsize = 0;
	std::cout << "Compress DNA file....\n";
	std::string dnafilename = savePath + "packedDNA.txt";
	std:: string orginfile  = savePath+"DNAfile.txt";
	std::string seqlengthpath = savePath + "seqlength.txt";
	FM *fmdna = new FM(orginfile.c_str(), 256, 256,shape, 1,1/*code method*/, true/*globalArgs.calsample*/); 
	std::cout << "DNA compress complete. "<<endl;	
	std::string filename2 = dnafilename;
	filename2 += ".CMP.txt";
	string pathdna = filename2 + SAL; 
	if(seqlength==1)
		fmdna->GetEnterPos();
	fmdna->Save(filename2.c_str());
	fmdna->SaveSAL(pathdna.c_str(),1);
	std::cout << pathdna << endl;
	std :: ofstream fout;
	fout.open(seqlengthpath,std :: ios_base :: out | std :: ios_base :: trunc);
	fout<<seqlength<<endl<<dna__len;
	fout.close();
	return;
}
void FTcomp::paraCMPQV(int blockSize, int shape, std :: string savePath)
{
	std::cout << "Compress QV files....\n";
	preQV(savePath + "QVfile.txt", savePath + "preQVfile.txt");
	int partsize = 0;
	std::string qvfilename = savePath + "preQVfile.txt";
	FM *testqv = new FM(qvfilename.c_str(), 256, 256, shape, 1,0/*code method*/, true/*globalArgs.calsample*/);
	partsize = testqv->SizeInByte() / 1024;
	std::cout << "QV compress complete."<<endl;
	string filename2 = qvfilename;
	filename2 += ".CMP.txt";
	testqv->GetEnterPos();
	testqv->Save(filename2.c_str());
	string pathqua = filename2 + SAL;
	testqv->SaveSAL(pathqua.c_str(),0);
	cout << pathqua << endl;
	return;
}
void FTcomp::GetIDindex(string cmpid, string savePath)
{
	//decompress ID
	v_pos.resize(1,0);
	v_length.resize(1,0);
	bool lenflag = getIDfileds(cmpid, v_FMid, v_IAid, v_strid, v_strsplitid, v_pos, v_length,ftcomp_first_id_seq);
	return;
}
void FTcomp::LoadIDSAL(string cmpid)
{
	for (int i = 0; i < v_FMid.size(); i++)
	{
		if (v_FMid[i] != 0)
		{
			string strpath(cmpid);
			strpath = cmpid + "4_" + to_string(i) + "_" + to_string(v_length[i]) + ".txt.CMP.txt.SAL";
			v_FMid[i]->LoadSAL(strpath.c_str());
		}
	}
}
void FTcomp::GetID(int index,string & strID)
{
	for (i64 i = 0 ;i< v_pos.size();i++){
		if (v_pos[i]== 0)
			strID += v_strid[i];
		else if (v_pos[i] == 1){
			InArray* in = v_IAid[i];
			auto val =in->GetValue(index);
			strID += to_string(val);
		}
		else if(v_pos[i]==2){
			FM *f = v_FMid[i];
			int length = v_length[i];
			auto pchar = f->Extracting(index * length,length);
			string strfm((const char*)pchar);
			strID += strfm;
		}
		strID += v_strsplitid[i];
	}
}
void FTcomp::DeleteSameLine3(char D, i64 *dnaposvec, i64 num) //
	{
		FM *fmp;
		if (dna_fix_len==0)
			for (i64 i =0;i<num;i++)
			{
				dnaposvec[i] = (dnaposvec[i] / (dna_len+1));
			}
		else
		{
			if (D == 'q' || D == 'Q')
					fmp = fmqua;
			else
			{
					fmp = fmdna;
			}
				// cout << dnaposvec.size() << endl;
			for (i64 i =0;i<num;i++)
			{
					// cout<<i<<endl;
				dnaposvec[i] = (fmp->GetEnterPosRank(dnaposvec[i]));
			}
		}
		// FILEANDLINE;
		// cout<<dnaposvec[0]<<":"<<dnaposvec[1]<<endl;
		return ;
	}

void FTcomp::GetFASTQ(string pattern,vector<string> & result)//wch get
{
	// vector<string> result;
	i64 num = 0;
	i64 startpos =0,endpos = 1,startpos1=0,endpos1 =1;
	auto lines = GetDnaPatternLine(pattern, num);
	// i64 *enterposdna;
	// i64 sizedna;
	if(dna_fix_len == 1)
	{
		Dmap *enterposdna = fmdna->GetEnterPos();
		Dmap *enterposqua = fmqua->GetEnterPos();
		// auto start = enterposdna.begin();
		// auto size = enterposdna.size();
		// auto lines = DeleteSameLine(dnaposvec, enterposdna);
		result.resize(num);
		int cnt =0;
		for(int j = 0 ;j<num;j++)
		{
			u64 lineno = lines[j];
			u64 startposdnacur = 0;
			if (lineno >= 1)
				startposdnacur = enterposdna->select(lineno) + 1; ///*+1 for skip /n*/
			auto endposdnacur = enterposdna->select(lineno+1);

			u64 startposquacur = 0;
			if (lineno >= 1)
				startposquacur = enterposqua->select(lineno) + 1; ///*+1 for skip /n*/
			auto endposquacur = enterposqua->select(lineno+1);

			unsigned char *pchardna = fmdna->Extracting(startposdnacur, endposdnacur - startposdnacur);
			unsigned char *pchar = fmqua->Extracting(startposquacur, endposquacur - startposquacur);
			// cout<<__FILE__<<"."<<__LINE__<<":"<<i<<","<<endpos<<";"<<startpos<<"len:"<<endpos - startpos<<endl;
			string str((char *)pchar);
			string strdna((char *)pchardna);
			string strID(ftcomp_first_id_seq);
			if(lineno >= 1)
			{
				strID = "";
				GetID(lineno-1, strID);
				if(strID[strID.size()-1]<'0'||strID[strID.size()-1]>'9')
					strID=strID.substr(0,strID.length()-1);
			}
			// string strID = IDS[lines[j] - 1];
			result[cnt++] = strID + "\n" + strdna + "\n+\n" + deBytetwo(str);
			// result[cnt++]=(make_tuple(strID, strdna, deBytetwo(str)));
		}
	}
	else
	{
		result.resize(num);
		int cnt =0;
		Dmap *enterposqua = fmqua->GetEnterPos();
		for(int j = 0 ;j<num;j++)
		{
			u64 lineno = lines[j];
			u64 startposdnacur = 0;
			if (lineno >= 1)
				startposdnacur = lineno*(dna_len+1); ///*+1 for skip /n*/

			u64 startposquacur = 0;
			if (lineno >= 1)
				startposquacur = enterposqua->select(lineno) + 1; ///*+1 for skip /n*/
			auto endposquacur = enterposqua->select(lineno+1);

			unsigned char *pchardna = fmdna->Extracting(startposdnacur, dna_len);
			unsigned char *pchar = fmqua->Extracting(startposquacur, endposquacur-startposquacur);
			// cout<<__FILE__<<"."<<__LINE__<<":"<<i<<","<<endpos<<";"<<startpos<<"len:"<<endpos - startpos<<endl;
			string str((char *)pchar);
			string strdna((char *)pchardna);
			string strID(ftcomp_first_id_seq);
			if(lineno >= 1)
			{
				strID = "";
				GetID(lineno-1, strID);
				if(strID[strID.size()-1]<'0'||strID[strID.size()-1]>'9')
					strID=strID.substr(0,strID.length()-1);
			}
			// string strID = IDS[lines[j] - 1];
			result[cnt++] = strID + "\n" + strdna + "\n+\n" + deBytetwo(str);
			// result[cnt++]=(make_tuple(strID, strdna, deBytetwo(str)));
		}
	}
	// return std::move(result);
		// return result;
}

i64 * FTcomp::GetDnaPatternLine(string pattern, i64 &num)
{
	i64 *pp = fmdna->Locating(pattern.c_str(),  num);
	DeleteSameLine3('d', pp, num);
	return pp;
}

i64 FTcomp::GetDnaPatternCount(string pattern)
{
	i64 num = 0;
	fmdna->Counting(pattern.c_str(), num);
	return (num);
}

void FTcomp::compFastqFile(std :: string fastqfile,  std :: string savePath)
{
	//clock_t start_clock1 = clock();
	struct timeval start, end;
	gettimeofday(&start,NULL);

	std :: vector<std :: string> ID;
	std::string DNAfile = savePath + "DNAfile.txt";
	std::string QVfile = savePath + "QVfile.txt";
	int dna__len = 0;
	seqlength = create_text(fastqfile,ID,DNAfile,QVfile,dna__len);
	for (int i = 0; i < 3; i++)
	{
		
		if (i == 0)
		{
			paraCMPID(ID,256,256, 2,savePath);//liuxi shape
		}
		else if (i == 1){
			paraCMPDNA(seqlength,256,1,savePath,dna__len);
		}
		else{
			paraCMPQV(256, 1, savePath);
		}

	}
	
	gettimeofday(&end,NULL);
	double timeUsed = (((long)end.tv_sec) * 1000 + ((long)end.tv_usec) / 1000 - ((long)start.tv_sec) * 1000 + ((long)start.tv_usec) / 1000) / 1000.0;
	cout << "压缩时长 : " << timeUsed << "s"<<endl;
	ofstream fout;
	fout.open("./OUTPUT/Time_Record.txt", ios_base::out | ios_base::app);
	fout << fastqfile << endl;
	fout << "压缩时长 : " << timeUsed << "s"<< endl;
	fout.close();
	return;
}
void FTcomp::saveID(std ::string cmpid)
{
	// fmdna = buildIndexFromCmpfile(cmpdna, savePath, SampleMethod::equidistant);
	for (int i = 0; i < v_FMid.size();i++){
		if (v_FMid[i] != 0){
			string strpath(cmpid);
			strpath = cmpid +"4_" + to_string(i) + "_" + to_string(v_length[i]) + ".txt.CMP.txt.SAL";
			v_FMid[i]->BuildIndex(SampleMethod::equidistant);
			v_FMid[i]->SaveSAL(strpath.c_str(),0);
		}

	}
}
int a_to_i(string x)
{
	int i = 0;
	for(int j = 0;j < x.length();++j)
	{
		i *= 10;
		i += x[j]-'0';
	}
	return i;
}

string FTcomp::DNAextract(u64 lineno)
{
    if(dna_fix_len == 0)
    {
        u64 startposdnacur=0;
        if (lineno >=1)
            startposdnacur = lineno*(dna_len+1); ///*+1 for skip /n*/
        unsigned char *pchardna = fmdna->Extracting(startposdnacur, dna_len);
        string strdna =string((char *)pchardna);
        return strdna;
    }
    else
    {
        Dmap* enterposdna = fmdna->GetEnterPos();
        u64 startposdnacur=0;
        if (lineno >=1)
            startposdnacur = enterposdna->select(lineno) + 1; ///*+1 for skip /n*/
        auto endposdnacur = enterposdna->select(lineno+1);
        unsigned char *pchardna = fmdna->Extracting(startposdnacur  , endposdnacur - startposdnacur);
        string strdna =string((char *)pchardna);
        return strdna;
    }
}

void FTcomp::LoadIndexFromCompressedFile(std ::string cmpid, std ::string cmpdna, std ::string cmpqv, std ::string savePath)
{
	struct timeval start, end;
	gettimeofday(&start, NULL);
	string idfilename=savePath + "seqlength.txt";
	ifstream fin1;
	fin1.open(idfilename.c_str(),ios_base::in);
	string fixlen="";
	getline(fin1,fixlen);
	if(fixlen=="1")
		dna_fix_len=1;
	else if(fixlen=="0")
	{
		fixlen="";
		getline(fin1,fixlen);
		dna_fix_len=0;
		dna_len=a_to_i(fixlen);
		cout<<dna_len<<endl;
	}
	fin1.close();
	// #pragma omp parallel for num_threads(3) //下面大括号内部为并行区域
	for (int i = 0; i < 3; i++)
	{
		if (i == 0){
			GetIDindex(cmpid, savePath);
			LoadIDSAL(cmpid);
		}
		else if (i == 1)
		{
			fmdna = buildIndexFromCmpfile(cmpdna, savePath);
		}
		else
			fmqua = buildIndexFromCmpfile(cmpqv, savePath);
	}
#pragma omp barrier
	gettimeofday(&end, NULL);
	double timeUsed = (((long)end.tv_sec) * 1000 + ((long)end.tv_usec) / 1000 - ((long)start.tv_sec) * 1000 + ((long)start.tv_usec) / 1000) / 1000.0;

	cout << "载入时长 : " << timeUsed << "s"<<endl;
	return;
}
FM *FTcomp::buildIndexFromCmpfile(string cmpDNA, string savePath)
{
	std::string seqlengthpath = savePath + "seqlength.txt"; //read dna length

	std ::ifstream fin;
	fin.open(seqlengthpath, std ::ios_base ::in);
	if (!fin.is_open())
	{
		std ::cout << "Can't open the file\n";
		exit(EXIT_FAILURE);
	}
	fin >> seqlength;

	FM *fmdna = new FM();
	fmdna->Load(cmpDNA.c_str());
	std ::string path = cmpDNA + SAL;
	cout<<__FILE__<<__LINE__<<endl;
	cout<<path<<endl;
	fmdna->LoadSAL(path.c_str());
	i64* vec;
	i64 num;

	builded = true;
	// i64 num = 0;

	return fmdna;
}