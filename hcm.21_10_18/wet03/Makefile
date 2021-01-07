# Update the paths below according to your directory structure
HCMPATH=$(shell pwd)/../
MINISAT=$(HCMPATH)/minisat

# required for adding code of minisat to your program
MINISAT_OBJS=$(MINISAT)/core/Solver.o $(MINISAT)/utils/Options.o $(MINISAT)/utils/System.o $(HCMPATH)/flattener/flat.o

CXXFLAGS=-ggdb -O0 -fPIC -I$(HCMPATH)/include -I$(MINISAT) -I$(HCMPATH)/flattener
CFLAGS=-ggdb -O0 -fPIC -I$(HCMPATH)/include -I$(MINISAT) -I$(HCMPATH)/flattener
CC=g++ -g
LDFLAGS=$(MINISAT_OBJS) -L$(HCMPATH)/src -lhcm -Wl,-rpath=$(HCMPATH)/src 

all: gl_verilog_fev

gl_verilog_fev: helper.o main.o 
	$(CC) -o $@ $^ $(LDFLAGS)

helper.o: helper.h helper.cpp

main.o: main.cpp ../flattener/flat.cc 

clean: 
	@ rm *.o gl_verilog_fev
