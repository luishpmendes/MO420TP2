#include <cassert>
#include <set>

#include "instance.hpp"
#include "mc_solver.hpp"

int main () {
    Instance inst;
    inst.load("../../instances/exemplo.gcc");

    MCSolver mcSolver;

    std::set<std::set<unsigned int>> maximalCliques = mcSolver.solve(&inst);

    assert(maximalCliques.size() == 8);

    std::set<unsigned int> Q1;
    Q1.insert(0);
    Q1.insert(1);
    Q1.insert(4);
    assert(maximalCliques.find(Q1) != maximalCliques.end());

    std::set<unsigned int> Q2;
    Q2.insert(0);
    Q2.insert(3);
    Q2.insert(4);
    assert(maximalCliques.find(Q2) != maximalCliques.end());

    std::set<unsigned int> Q3;
    Q3.insert(1);
    Q3.insert(2);
    Q3.insert(4);
    assert(maximalCliques.find(Q3) != maximalCliques.end());

    std::set<unsigned int> Q4;
    Q4.insert(2);
    Q4.insert(3);
    Q4.insert(4);
    assert(maximalCliques.find(Q4) != maximalCliques.end());

    std::set<unsigned int> Q5;
    Q5.insert(2);
    Q5.insert(5);
    assert(maximalCliques.find(Q5) != maximalCliques.end());

    std::set<unsigned int> Q6;
    Q6.insert(5);
    Q6.insert(6);
    Q6.insert(8);
    assert(maximalCliques.find(Q6) != maximalCliques.end());

    std::set<unsigned int> Q7;
    Q7.insert(6);
    Q7.insert(7);
    Q7.insert(8);
    assert(maximalCliques.find(Q7) != maximalCliques.end());

    std::set<unsigned int> Q8;
    Q8.insert(7);
    Q8.insert(9);
    assert(maximalCliques.find(Q8) != maximalCliques.end());

    return 0;
}

