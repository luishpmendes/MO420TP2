#include "ilcplex/ilocplex.h"
#include "instance.hpp"
#include "global.hpp"

ILOUSERCUTCALLBACK2(Cuts, IloBoolVarArray, x, const Instance &, inst) {
  IloEnv env = getEnv();

  IloNumArray val(env);
  getValues(val, x);

  NodeId node = getNodeId();
  double lpobjval = getObjValue();
  std::cerr << "generating cuts" << std::endl;

  if(gInfo.iterSep == 0) {
    gReport.firstDualBound = lpobjval;
  }

  if(gInfo.firstNode && isAfterCutLoop()) {
    gInfo.firstNode = false;
    gReport.rootDualBound = lpobjval;
  }
  gInfo.iterSep++;

}
