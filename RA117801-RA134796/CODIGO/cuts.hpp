#include "ilcplex/ilocplex.h"
#include "instance.hpp"
#include "global.hpp"
#include "occut.hpp"
#include "stcut.hpp"

ILOUSERCUTCALLBACK2(Cuts, IloBoolVarArray, x, const Instance &, inst) {
  IloEnv env = getEnv();

  IloNumArray val(env);
  getValues(val, x);

  std::vector<double> valVecDouble(inst.edges.size());
  for(int i = 0; i < inst.edges.size(); i++)
    valVecDouble[i] = (double)val[i];

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

  for(const std::vector<int> & w : getSubTours(inst, valVecDouble)) {
      IloExpr lhs(env);
      //find edges between vertices of w
      for(int i = 0; i < w.size(); i++) {
        for(int j = i+1; j < w.size(); j++) {
          int u = w[i];
          int v = w[j];
          std::map<Edge, int>::const_iterator it = inst.edge2id.find(Edge(u, v));
          if(it != inst.edge2id.end())//found
            lhs  += x[it->second];
        }
      }
      int rhs = w.size()-1;
      add(lhs <= rhs);
      gReport.totalSTCuts++;
  }

  std::set<std::set<unsigned int>> oddCycles;

  occuts(val, inst, oddCycles);

  for (std::set<std::set<unsigned int>>::iterator it = oddCycles.begin(); it != oddCycles.end(); it++) {
    std::set<unsigned int> oddCycle = (*it);

    IloExpr lhs (env);
    double rhs = (((double) oddCycle.size()) - 1.0) / 2.0;

    for (std::set<unsigned int>::iterator it2 = oddCycle.begin(); it2 != oddCycle.end(); it2++) {
      unsigned int i = (*it2);

      lhs += x[i];
    }

    add(lhs <= rhs);
    gReport.totalOCCuts++;
  }
}

