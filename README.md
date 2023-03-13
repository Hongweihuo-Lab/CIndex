# CIndex: Compressed indexes for fast retrieval of FASTQ files 
CIndex[1] is a compressed index for FASTQ files. It uses the Burrows-Wheeler transform[2] and the wavelet tree[3], combined with hybrid encoding and succinct data structures, to achieve minimal space usage while enabling fast retrieval on the compressed FASTQ files.  
CIndex has been adapted to be a general-purpose text index, [CIndexGeneral](https://github.com/Hongweihuo-Lab/CIndexGeneral). It uses compiler c++ 11, gcc 5.4 or higher, with sse4.2, and Ubuntu16.04.1 LTS 64.
## Compiling 
    $ git clone https://github.com/Hongweihuo-Lab/CIndex.git
    $ cd Cindex
    $ make
## Usage
By entering different commands after –z, you can enter different operating modes.  
The operation modes include: build, cr, lr, er, efq, which correspond to the construction of compressed indexes, reads counting, reads positioning, reads extraction, and fastq extraction, respectively.
### Index Construction
Create the compressed index of the FASTQ file.   
The command to create the compressed index for the FASTQ file is:   

| Command | Description |
| --- | --- |
|./CIndex -f fastqpath -z build |  -f fastqpath is the path of the FASTQ file to be compressed. 
  |  |   -z build means to build the compressed index.   

The index file will be generated under the folder corresponding to the FASTQ file after the operation is completed.
### Queries
CIndex supports four queries: CountR (cr), LocateR (lr), ExtractR (er), extractFASTQ (efq), listed as follows:

| Command | Description |
| --- | --- |
| ./CIndex –f fastqpath –z cr   | Input P, it returns the number of occurrences of pattern P in reads strings R. 
| ./CIndex –f fastqpath –z lr   | Input P, it reports all the read line numbers that contain an occurrence of P.
| ./CIndex –f fastqpath –z er   | Input read line number, it extracts the read associated with the given read line number.
| ./CIndex -f fastqpath -z efq  | Input P, it extracts the collection of records that contain P.

## Paper
[1] H. Huo, P. Liu, C. Wang, H. Jiang, and J. S. Vitter, CIndex: Compressed indexes for fast retrieval of FASTQ files, Bioinformatics, September 15, 2021,  [PDF](https://doi.org/10.1093/bioinformatics/btab655). 

## ChangeLog 
2022.6.15: fix a bug in BitMap.h.
