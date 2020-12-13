HCMPATH=$(shell pwd)/../

CXXFLAGS=-ggdb -O0 -fPIC -I$(HCMPATH)/include -I$(HCMPATH)/flattener -I$(HCMPATH)/sigvec -I$(HCMPATH)/hcm_vcd
CFLAGS=-ggdb -O0 -fPIC -I$(HCMPATH)/include -I$(HCMPATH)/flattener -I$(HCMPATH)/sigvec -I$(HCMPATH)/hcm_vcd
CC=g++ -g -std=c++11
LDFLAGS=-L$(HCMPATH)/src -lhcm -Wl,-rpath=$(HCMPATH)/src  -L$(HCMPATH)/sigvec -lhcmsigvec  -Wl,-rpath=$(HCMPATH)/sigvec -L$(HCMPATH)/hcm_vcd

all: event_sim

event_sim: helper.o HW2ex1.o
	$(CC) -o $@ $^ $(LDFLAGS) $(HCMPATH)/flattener/flat.o $(HCMPATH)/hcm_vcd/vcd.o
	cp event_sim

helper.o: helper.h helper.cc

clean:
	 @ rm *.o event_sim
