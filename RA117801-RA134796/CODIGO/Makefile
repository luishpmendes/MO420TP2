cplex = /opt/ibm/ILOG/CPLEX_Studio128/cplex
concert = /opt/ibm/ILOG/CPLEX_Studio128/concert

dflags  = -DIL_STD -O0 -g -Wall -fno-inline -std=c++11
rflags  = -DIL_STD -o3 -std=c++11

iflags = -I$(cplex)/include -I$(concert)/include
lflags  = -L$(cplex)/lib/x86-64_linux/static_pic -L$(concert)/lib/x86-64_linux/static_pic -lconcert -lilocplex -lcplex -lpthread -ldl

all_tests: instance_test report_test sp_solver_test bnc

instance_test: instance_test.cpp instance.hpp
	g++ $(dflags) instance_test.cpp $(iflags) -o instance_test

report_test: instance_test report_test.cpp report.hpp  solution.hpp
	g++ $(dflags) report_test.cpp $(iflags) -o report_test

sp_solver_test: instance_test sp_solver.hpp sp_solver_test.cpp
	g++ $(dflags) sp_solver_test.cpp $(iflags) -o sp_solver_test

bnc : bnc.cpp report_test instance_test sp_solver_test  solver.cpp cuts.hpp stcut.hpp occut.hpp global.hpp params.hpp primalheur.hpp
	g++ $(dflags) bnc.cpp solver.cpp $(iflags) $(lflags) -o bnc 

clean:
	rm -f bnc instance_test report_test sp_solver_test *.o

