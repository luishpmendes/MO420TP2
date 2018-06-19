#include "ilcplex/ilocplex.h"
#include "instance.hpp"
#include "global.hpp"
#include "occut.hpp"
#include "stcut.hpp"

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

  std::vector<IloExpr> OCcut(0), STcut(0);
  std::vector<double> OCrhs(0), STrhs(0);

  occuts(env, x, val, inst, OCcut, OCrhs);
  stcuts(env, x, val, inst, STcut, STrhs);

  for (int i = 0; i < (int) OCcut.size(); i++) {
      add(OCcut[i] <= OCrhs[i]);
      gReport.totalOCCuts++;
  }

  for (int i = 0; i < (int) STcut.size(); i++) {
      add(STcut[i] <= STrhs[i]);
      gReport.totalSTCuts++;
  }
}
