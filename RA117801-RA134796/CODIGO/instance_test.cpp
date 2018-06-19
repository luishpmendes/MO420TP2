#include "instance.hpp"
#include <cassert>

int main(int argc, char  ** argv) {
  Instance inst;
  inst.load("../../instances/exemplo.gcc");

  assert(inst.name=="exemplo.gcc");
  assert(inst.size==10);
  assert(inst.edges.size()==15);
  assert(inst.conflicts.size()==5);

  assert(inst.edges[2]==Edge(0,4));
  assert(inst.edge2id[Edge(0,4)]==2);
  assert(inst.cost[2]==6);

  assert(inst.conflicts[1].first==1);
  assert(inst.conflicts[1].second==8);
  assert(inst.conflictsByEdge[12].size()==2);
  assert(inst.conflictsByEdge[12][0]==3);

  assert(inst.minWeightEdgeId==4);
}



