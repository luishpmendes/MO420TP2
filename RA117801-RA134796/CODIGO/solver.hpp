#pragma once
#include "ilcplex/ilocplex.h"
#include "instance.hpp"
#include "params.hpp"

class BNCSolver {
  protected:
  IloEnv _env;
  IloModel _model;
  IloCplex _cplex;

  IloBoolVarArray _x;
  IloExpr _obj;
  Params _params;

  Instance _inst;
  //Params _params;
  void createPairConflicts();
  void createCliqueConflicts();
  void createBaseModel();
  void createCuts();
  void createPrimalHeuristic();

  void config();
  public:
  void solve(const Instance & inst, const Params & params);//, const Params & params;
};  
