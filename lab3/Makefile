CFLAGS=-g -fPIC -m64 -Wall
LFLAGS=-fPIC -m64 -Wall
MPICC=mpicc

all: lab3

lab3.o: lab3.c
	$(MPICC) $(CFLAGS) -c lab3.c -o $@

lab3: lab3.o
	$(MPICC) $(LFLAGS) lab3.o -o lab3

ps:
	ps -fu $$USER

clean:
	rm lab3
	rm lab3.o