PROG = lab3
CFLAGS = -g -fPIC -m64 -Wall -std=c99
LFLAGS= -fPIC -m64 -Wall
CC = mpicc

all: $(PROG)

lab3: lab3.o seq.o par.o
	$(CC) $(LFLAGS) lab3.o seq.o par.o -o lab3

lab3.o: lab3.c seq.h par.h
	$(CC) $(CFLAGS) -c lab3.c

seq.o: seq.c seq.h
	$(CC) $(CFLAGS) -c seq.c

par.o: par.c par.h
	$(CC) $(CFLAGS) -c par.c

run:
	mpirun -np $6 lab3

clean:
	/bin/rm -f *~
	/bin/rm -f *.o
