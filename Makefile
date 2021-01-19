CC=gcc
CFLAGS=-I./include
DEPS = include/rfs.h fs/rfs.c


obj/rfs: obj/rfs.o
	$(CC) $(CFLAGS) -o rfs obj/rfs.o

obj/rfs.o: fs/rfs.c include/rfs.h
	mkdir -p obj
	$(CC) $(CFLAGS) -c fs/rfs.c -o obj/rfs.o

