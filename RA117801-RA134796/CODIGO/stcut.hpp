#include "ilcplex/ilocplex.h"
#include "instance.hpp"
#include "global.hpp"
#include "mf_solver.hpp"

std::vector<std::vector<int>> getSubTours(const Instance & inst,
    const std::vector<double> & frac) {
  std::vector<std::vector<int>> subtours;

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
    subtours.push_back(h.getCutSide(0));
  }
  
  return subtours;
}
