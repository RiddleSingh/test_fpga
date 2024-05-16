# -*- Makefile -*-
#INCLUDES= -I ../C-Libraries/gsl-1.15_universal/include -I .
CPUCC  = g++
CFLAGS   = -Wall
LDFLAGS  = -lm -lgsl -lgslcblas
#GSLLIB = -L../C-Libraries/gsl-1.15_universal/include/
NAMEEXEC = app

$(NAMEEXEC): main.o
	$(CPUCC) main.o -o $(NAMEEXEC) $(GSLLIB) $(LDFLAGS)
main.o: main.c auxiliary.h usr_decl.h perf.h
	$(CPUCC) -c -O3 main.c
clean:
	rm *.o $(NAMEEXEC)
