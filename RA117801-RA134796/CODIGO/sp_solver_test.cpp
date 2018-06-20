#include <cassert>
#include <vector>

#include "instance.hpp"
#include "sp_solver.hpp"

int main () {
    Instance inst;
    inst.load("../../instances/exemplo.gcc");

    std::vector<double> d;
    std::vector<int> pi;

    DijkstraSPSolver spSolver;
    spSolver.solve(&inst, 0, 9, d, pi);

    assert(d[1] == 1);
    assert(pi[1] == 0);

    assert(d[2] == 5);
    assert(pi[2] == 1);

    assert(d[5] == 6);
    assert(pi[5] == 2);

    assert(d[6] == 8);
    assert(pi[6] == 5);

    assert(d[7] == 8);
    assert(pi[7] == 6);

    assert(d[9] == 10);
    assert(pi[9] == 7);
}

