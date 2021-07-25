CC=g++
# SIZE_FLAGS = -DMASSIVE_DATA_RLCSA
CFLAGS=  $(SIZE_FLAGS) -std=c++11 -fopenmp  -w -O3 -msse4.2
# CFLAGS=  $(SIZE_FLAGS) -std=c++11 -fopenmp -w   -g -msse4.2
OBJS = sampler.o ABS_WT.o BitMap.o create_newLevel.o create_ref.o preMethod.o  parameters.o utils.o  \
	CreateText.o DecodeGamma.o DNAcmp.o EliasCode.o find_consor.o \
	flow.o  Balance_WT.o Huffman_WT.o Hutacker_WT.o IDcmp.o InArray.o loadkit.o \
	QVcmp.o reverseFile.o savekit.o saveTree.o statics.o UseCount.o  \
	divsufsort.o sssort.o trsort.o utils.o BaseClass.o FM.o
my_fastq: div fm.a  main.o
	$(CC)  -fopenmp main.o fm.a -o CIndex

beetl: div fm.a  beetl.o
	$(CC)  -fopenmp beetl.o fm.a -o fm_beetl_eq

# fm.a:
# 	ar rc  fm.a $^
fm.a: $(OBJS)
	ar rc fm.a $(OBJS)

beetl2.o:main_beetl.cpp
		$(CC) -c $(CFLAGS) $< -o $@

beetl.o:mainBeetl.cpp
		$(CC) -c $(CFLAGS) $< -o $@

mult.o:MainMulti.cpp
		$(CC) -c $(CFLAGS) $< -o $@
%.o:%.cpp *.h
	$(CC) -c  $(CFLAGS) $< -o $@
# BooPHF.o:BooPHF.h
	# $(CC) -c  $(CFLAGS) $< -o $@
main.o : main.cpp
	$(CC) -c $(CFLAGS) $< -o $@

# divsufsort64.o :divsufsort64.h
# 	$(CC) $(CFLAGS) $< -o $@

clean:
	rm my_fastq *.a *.o ./divsufsort/*.a ./divsufsort/*.o

div:
	make -C ./divsufsort/; cp divsufsort/libdivsufsort.a .;ar x libdivsufsort.a;rm libdivsufsort.a
