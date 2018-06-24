#include "ilcplex/ilocplex.h"
#include "instance.hpp"
#include "global.hpp"
#include "occut.hpp"
#include "stcut.hpp"

ILOUSERCUTCALLBACK3(UserCuts, IloBoolVarArray, x, const Instance &, inst,
    bool, useOCI) {
  IloEnv env = getEnv();

  IloNumArray val(env);
  getValues(val, x);

  std::vector<double> valVecDouble(inst.edges.size());
  for(int i = 0; i < inst.edges.size(); i++)
    valVecDouble[i] = (double)val[i];

  NodeId node = getNodeId();
  double lpobjval = getObjValue();

  if(gInfo.iterSep == 0) {
    gReport.firstDualBound = lpobjval;
  }

  if(gInfo.firstNode && isAfterCutLoop()) {
    gInfo.firstNode = false;
    gReport.rootDualBound = lpobjval;
  }
  gInfo.iterSep++;

  std::set<std::vector<int>> subtours = getSubTours(inst, valVecDouble);
  for(const std::vector<int> & w :  subtours) {
      IloExpr lhs(env);
      //find edges between vertices of w
      double dlhs = 0;
      for(int i = 0; i < w.size(); i++) {
        for(int j = i+1; j < w.size(); j++) {
          int u = w[i];
          int v = w[j];
          std::map<Edge, int>::const_iterator it = inst.edge2id.find(Edge(u, v));
          if(it != inst.edge2id.end()) {//found
            lhs  += x[it->second];
            dlhs += valVecDouble[it->second];
          }
        }
      }
      int rhs = w.size()-1;
      if(dlhs > rhs+EPS) {
        add(lhs <= rhs);
      }
      gReport.totalSTCuts++;
  }

  std::set<std::set<unsigned int>> oddCycles;

  if(useOCI) {
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
}

ILOLAZYCONSTRAINTCALLBACK3(LazyCuts, IloBoolVarArray, x, const Instance &, inst,
    bool, useOCI) {
  IloEnv env = getEnv();

  IloNumArray val(env);
  getValues(val, x);

  std::vector<double> valVecDouble(inst.edges.size());
  for(int i = 0; i < inst.edges.size(); i++)
    valVecDouble[i] = (double)val[i];

  NodeId node = getNodeId();
  double lpobjval = getObjValue();

  if(gInfo.iterSep == 0) {
    gReport.firstDualBound = lpobjval;
  }

  if(gInfo.firstNode) {
    gInfo.firstNode = false;
    gReport.rootDualBound = lpobjval;
  }
  gInfo.iterSep++;

  std::set<std::vector<int>> subtours = getSubTours(inst, valVecDouble);
  for(const std::vector<int> & w :  subtours) {
      IloExpr lhs(env);
      double dlhs=0;
      //find edges between vertices of w
      for(int i = 0; i < w.size(); i++) {
        for(int j = i+1; j < w.size(); j++) {
          int u = w[i];
          int v = w[j];
          std::map<Edge, int>::const_iterator it = inst.edge2id.find(Edge(u, v));
          if(it != inst.edge2id.end()) {//found
            dlhs += valVecDouble[it->second];
            lhs  += x[it->second];
          }
        }
      }
      int rhs = w.size()-1;
      add(lhs <= rhs);
      gReport.totalSTCuts++;
  }

  std::set<std::set<unsigned int>> oddCycles;

  if(useOCI) {
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
}

