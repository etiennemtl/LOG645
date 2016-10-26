PROG = lab3
CFLAGS = -g -fPIC -m64 -Wall -std=c99
LFLAGS= -fPIC -m64 -Wall
CC = mpicc

all: $(PROG)

lab3.o: lab3.c
	$(CC) $(CFLAGS) -c lab3.c

lab3: lab3.o
	$(CC) $(LFLAGS) lab3.o -o lab3

run:
	mpirun lab3

clean:
	/bin/rm -f *~
	/bin/rm -f *.o
