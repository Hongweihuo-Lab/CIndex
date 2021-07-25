# CIndex: a compressed index for fast retrieval of FASTQ files
CIndex[1] is a compressed index for FASTQ files. It uses the Burrows-Wheeler transform[2] and the wavelet tree[3], combined with hybrid encoding and succinct data structures, to achieve minimal space usage while enabling fast retrieval on the compressed FASTQ files.    
CIndex uses compiler c++ 11, g++ 5.4 and above, with sse4.2.
## Compiling 
    $ git clone https://github.com/Hongweihuo-Lab/CIndex.git
    $ cd Cindex
    $ make
## Usage
By entering different commands after –z, you can enter different operating modes.  
The operation modes include: build, bindex, cr, lr, er, efq, which correspond to the construction of compressed indexes, reads counting, reads positioning, reads extraction, and fastq extraction, respectively.
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
|./CIndex –f fastqpath –z cr | Input P, it returns the number of occurrences of pattern P in reads strings R. 
|./ CIndex –f fastqpath –z lr | Input P, it reports all the read line numbers that contain an occurrence of P.
|./CIndex –f fastqpath –z er | Input the read line number, it extracts the read associated with the given read line number.
|./CIndex -f fastqpath -z efq | Input P, it extracts the collection of records that contain P.

## Reference
[1] H. Huo, P. Liu, C. Wang, H. Jiang, and J. S. Vitter, CIndex: a compressed index for fast retrieval of FASTQ files, submitted.   
[2] M. Burrows and D.J. Wheeler, A block-sorting lossless data compression algorithm, Tech. Report SRC-RR-124, Digital Equipment Corporation, Palo Alto, CA, 1994.   
[3] R. Grossi, A. Gupta, and J. S. Vitter, “High-order entropy-compressed text indexes,” In Proceedings of the 14th annual ACM-SIAM symposium on Discrete Algorithms, 2003, pp. 841–850.    

