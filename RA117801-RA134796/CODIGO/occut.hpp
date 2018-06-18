#include "ilcplex/ilocplex.h"
#include "instance.hpp"

ILOUSERCUTCALLBACK2(OCCut, IloBoolVarArray, x, const Instance &, inst) {
  IloEnv env = getEnv();

  IloNumArray val(env);
  getValues(val, x);

  NodeId node = getNodeId();
  double lpobjval = getObjValue();
  //%TODO
  std::cerr << "oc cut" << std::endl;
}
