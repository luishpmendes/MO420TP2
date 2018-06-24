cplex = /opt/ibm/ILOG/CPLEX_Studio128/cplex
concert = /opt/ibm/ILOG/CPLEX_Studio128/concert

dflags  = -DIL_STD -O0 -g -Wall -fno-inline -std=c++11
rflags  = -DIL_STD -O3 -std=c++11

iflags = -I$(cplex)/include -I$(concert)/include
lflags  = -L$(cplex)/lib/x86-64_linux/static_pic -L$(concert)/lib/x86-64_linux/static_pic -lconcert -lilocplex -lcplex -lpthread -ldl

all_tests: instance_test report_test sp_solver_test mf_solver_test preprocessor_test mc_solver_test primalheur_test bnc

instance_test: instance_test.cpp instance.hpp
	g++ $(dflags) instance_test.cpp $(iflags) -o instance_test

report_test: instance_test report_test.cpp report.hpp  solution.hpp
	g++ $(dflags) report_test.cpp $(iflags) -o report_test

sp_solver_test: instance_test sp_solver.hpp sp_solver_test.cpp
	g++ $(dflags) sp_solver_test.cpp $(iflags) -o sp_solver_test

mf_solver_test: instance_test mf_solver.hpp mf_solver_test.cpp
	g++ $(dflags) mf_solver_test.cpp $(iflags) -o mf_solver_test

preprocessor_test: instance_test variable_fixer.hpp variable_fixer.cpp preprocessor_test.cpp
	g++ $(dflags) preprocessor_test.cpp variable_fixer.cpp $(iflags) -o preprocessor_test

mc_solver_test: instance_test mc_solver.hpp mc_solver_test.cpp
	g++ $(dflags) mc_solver_test.cpp $(iflags) -o mc_solver_test

primalheur_test: instance_test primalheur.hpp primalheur_test.cpp
	g++ $(dflags) primalheur_test.cpp $(iflags) $(lflags) -o primalheur_test

bnc : bnc.cpp report_test instance_test sp_solver_test mf_solver_test mc_solver_test preprocessor_test primalheur_test solver.cpp cuts.hpp stcut.hpp occut.hpp global.hpp params.hpp
	g++ $(rflags) bnc.cpp solver.cpp variable_fixer.cpp $(iflags) $(lflags) -o bnc 

clean:
	rm -f bnc instance_test report_test sp_solver_test mf_solver_test preprocessor_test mc_solver_test primalheur_test *.o

