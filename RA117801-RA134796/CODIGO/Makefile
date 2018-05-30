cplex = /opt/ibm/ILOG/CPLEX_Studio128/cplex
concert = /opt/ibm/ILOG/CPLEX_Studio128/concert
compile = g++ -DIL_STD
cflags  = -c -O0 -g -Wall -fno-inline -std=c++11
xcflags = $(cflags) -I$(cplex)/include -I$(concert)/include
lflags  = -L$(cplex)/lib/x86-64_linux/static_pic -L$(concert)/lib/x86-64_linux/static_pic -lconcert -lilocplex -lcplex -lpthread -ldl

%.o: %.cpp
	$(compile) -o $@ $^ $(xcflags)

bnc : bnc.o
	$(compile) -o bnc bnc.o $(lflags)

clean:
	rm -f bnc *.o

