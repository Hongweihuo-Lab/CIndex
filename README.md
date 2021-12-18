# CIndex: Compressed indexes for fast retrieval of FASTQ files 
CIndex[1] is a compressed index for FASTQ files. It uses the Burrows-Wheeler transform[2] and the wavelet tree[3], combined with hybrid encoding and succinct data structures, to achieve minimal space usage while enabling fast retrieval on the compressed FASTQ files.  
CIndex has been adapted to be a general-purpose text index, [CIndexGeneral](https://github.com/Hongweihuo-Lab/CIndexGeneral).    
It uses compiler c++ 11, gcc 5.4 or higher, with sse4.2, and Ubuntu16.04.1 LTS 64.
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

## References
[2] M. Burrows and D.J. Wheeler, A block-sorting lossless data compression algorithm, Tech. Report SRC-RR-124, Digital Equipment Corporation, Palo Alto, CA, 1994.   
[3] R. Grossi, A. Gupta, and J. S. Vitter, High-order entropy-compressed text indexes, In Proceedings of the 14th annual ACM-SIAM symposium on Discrete Algorithms, 2003, pp. 841–850.    
[4] R. Grossi, J. S. Vitter, and B. Xu, “Wavelet trees: From theory to practice,” In Proceedings of the 1st International Conference on Data Compression, Communications and Processing, 2011, pp. 210–221.  
[5] L. Foschini, R. Grossi, A. Gupta, and J. S. Vitter, “Fast compression with a static model in high-order entropy,” In Proceedings of the IEEE Data Compression Conference, 2004, pp. 62–71.   
[6]  H. Huo, L. Chen, H. Zhao, J. S. Vitter, and Y. Nekrich, “A data-aware FM-index,” In Proceedings of the 17th Annual Workshop on Algorithm Engineering and Experiments, 2015, pp. 10–23.
