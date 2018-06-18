#pragma once
#include "ilcplex/ilocplex.h"
#include "instance.hpp"

ILOHEURISTICCALLBACK2(PrimalHeuristic, IloBoolVarArray, x, const Instance &, inst) {
  IloEnv env = getEnv();

  IloNumArray val(env);
  getValues(val, x);

  NodeId node = getNodeId();
  double lpobjval = getObjValue();

  //%TODO
  std::cerr << "primalHeuristic" << std::endl;
}
