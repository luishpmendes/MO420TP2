#include "mf_solver.hpp"
#include "constants.hpp"
#include <cassert>


void simpleTest() {
    Graph g(6);
    g.addFlowEdge(0, 1, 16 );
    g.addFlowEdge(0, 2, 13 );
    g.addFlowEdge(1, 2, 10 );
    g.addFlowEdge(1, 3, 12.7 );
    g.addFlowEdge(2, 1, 4 );
    g.addFlowEdge(2, 4, 14);
    g.addFlowEdge(3, 2, 9 );
    g.addFlowEdge(3, 5, 20 );
    g.addFlowEdge(4, 3, 7 );
    g.addFlowEdge(4, 5, 4.5);

    double flow = g.DinicMaxflow(0,5);
    assert(equalDouble(flow , 24.2));
}
int main(int argc, char ** argv) {
  simpleTest();

  return 0;
}
