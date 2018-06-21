#pragma once
#include "ilcplex/ilocplex.h"
#include "instance.hpp"
#include "params.hpp"
#include "solution.hpp"
#include <chrono>
#include "variable_fixer.hpp"

class BNCSolver {
  protected:
  IloEnv _env;
  IloModel _model;
  IloCplex _cplex;

  IloBoolVarArray _x;
  IloExpr _obj;
  Params _params;

  Instance _inst;
  
  Solution _solution;

  std::chrono::time_point<std::chrono::high_resolution_clock> startTime, endTime;

  std::set<int> _fixedOnes, _fixedZeros;
  std::vector<VariableFixer*> _preprocessors;
  bool _primalViable;

  void createPairConflicts();
  void createCliqueConflicts();
  void createBaseModel();
  void createCuts();
  void createPrimalHeuristic();

  void fixOne(int edge);
  void fixZero(int edge);

  void runPreprocessors();
  void config();

  void processSolution();
  void updateReport();
  public:
  void solve(const Instance & inst, const Params & params);
  bool isPrimalViable() const;
  Solution getSolution() const;
};  
