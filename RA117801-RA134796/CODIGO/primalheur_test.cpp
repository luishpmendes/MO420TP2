#include <cassert>
#include <set>
#include <vector>

#include "instance.hpp"
#include "primalheur.hpp"

int main () {
    Instance inst;
    inst.load("../../instances/exemplo.gcc");

    std::vector<double> x (inst.edges.size());
    x[0] = 0.0;
    x[1] = 0.0;
    x[2] = 0.0;
    x[3] = 0.5;
    x[4] = 1.0;
    x[5] = 1.0;
    x[6] = 0.0;
    x[7] = 1.0;
    x[8] = 1.0;
    x[9] = 0.5;
    x[10] = 1.0;
    x[11] = 0.5;
    x[12] = 0.5;
    x[13] = 1.0;
    x[14] = 1.0;

    std::set<int> fixedZeros;

    std::set<int> fixedOnes;
    fixedOnes.insert(7);
    fixedOnes.insert(14);

    std::set<int> primalSolution;

    bool result = fixSolution(inst, x, fixedZeros, fixedOnes, primalSolution);

    assert(result == true);

    assert(primalSolution.find(2) != primalSolution.end());
    assert(primalSolution.find(4) != primalSolution.end());
    assert(primalSolution.find(5) != primalSolution.end());
    assert(primalSolution.find(7) != primalSolution.end());
    assert(primalSolution.find(8) != primalSolution.end());
    assert(primalSolution.find(9) != primalSolution.end());
    assert(primalSolution.find(10) != primalSolution.end());
    assert(primalSolution.find(13) != primalSolution.end());
    assert(primalSolution.find(14) != primalSolution.end());

    return 0;
}

