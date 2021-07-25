CC=g++  -std=gnu++11
CFLAGS=-O3  -DHAVE_CONFIG_H -DBUILD_DIVSUFSORT64
#CFLAGS=-O3  -DHAVE_CONFIG_H
libdivsufsort.a:divsufsort.o sssort.o trsort.o utils.o
	ar rc libdivsufsort.a divsufsort.o sssort.o trsort.o utils.o

%.o:%.c *.h
	$(CC) -c $(CFLAGS) $< -o $@
clean:
	rm *.a *.o
