#include "ilcplex/ilocplex.h"
#include "instance.hpp"
#include "global.hpp"
#include "mf_solver.hpp"

std::set<std::vector<int>> getSubTours(const Instance & inst,
    const std::vector<double> & frac) {
  std::set<std::vector<int>> subtours;

  FlowGraph g(inst.size);
  for(int i = 0; i < inst.edges.size(); i++) {
    if(frac[i] < EPS)
      continue;
    g.addFlowEdge(inst.edges[i].u, inst.edges[i].v, frac[i]);
    g.addFlowEdge(inst.edges[i].v, inst.edges[i].u, frac[i]);
  }
  
  for(int t = 1; t < inst.size; t++) {
    FlowGraph h = g;
    double flow = h.DinicMaxflow(0, t);
    if(flow > 1-EPS)
      continue;
    std::vector<int> side = h.getCutSide(0);
    bool complement = false;

    if(side.size() < 3) {
      complement = true;
    }
    else {
      double wfrac = 0.0;
      for(int i = 0; i < side.size(); i++) {
        for(int j = i+1; j < side.size(); j++) {
          int u = side[i];
          int v = side[j];
          std::map<Edge, int>::const_iterator it = inst.edge2id.find(Edge(u, v));
          if(it != inst.edge2id.end()) {//found
            wfrac += frac[it->second];
          }
        }
      }
      if(wfrac < side.size()-1+EPS)
        complement = true;
    }
    if(complement) {
      std::vector<int> otherside;
      std::set<int> sideSet(side.begin(), side.end());
      for(int i = 0; i < inst.size; i++) {
        if(sideSet.count(i) == 0)
          otherside.push_back(i);
      }
      side = otherside;
    }
    subtours.insert(side);
  }
  
  return subtours;
}
