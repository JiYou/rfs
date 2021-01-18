CC=gcc
CFLAGS=-I../include
DEPS = include/rfs.h fs/rfs.c
OBJ = rfs.o

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

rfs: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)
