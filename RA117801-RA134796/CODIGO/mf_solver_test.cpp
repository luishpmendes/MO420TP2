#include "mf_solver.hpp"
#include "constants.hpp"
#include <cassert>


void simpleTest() {
    FlowGraph g(6);
    g.addFlowEdge(0, 1, 16 );
    g.addFlowEdge(0, 2, 13 );
    g.addFlowEdge(1, 2, 10 );
    g.addFlowEdge(1, 3, 11 );//testando multi arestas
    g.addFlowEdge(1, 3, 1.7 );//testando  ponto flutuante
    g.addFlowEdge(2, 1, 4 );
    g.addFlowEdge(2, 4, 14);
    g.addFlowEdge(3, 2, 9 );
    g.addFlowEdge(3, 5, 20 );
    g.addFlowEdge(4, 3, 7 );
    g.addFlowEdge(4, 5, 4.5);

    int s = 0;
    int t = 5;
    double flow = g.DinicMaxflow(s,t);
    assert(equalDouble(flow , 24.2));

    std::vector<int> truth = {0,1,2,4};
    std::vector<int> cutSide = g.getCutSide(s);

    for(int i = 0; i < truth.size(); i++) 
      assert(truth[i] == cutSide[i]);
}
int main(int argc, char ** argv) {
  simpleTest();

  return 0;
}
