#include "ilcplex/ilocplex.h"
#include "instance.hpp"

ILOUSERCUTCALLBACK2(STCut, IloBoolVarArray, x, const Instance &, inst) {
  IloEnv env = getEnv();

  IloNumArray val(env);
  getValues(val, x);

  NodeId node = getNodeId();
  double lpobjval = getObjValue();

  //%TODO
  std::cerr << "st cut" << std::endl;
}
