#include "variable_fixer.hpp"
#include "instance.hpp"
#include <cassert>

void lightEdgeTest() {
  Instance inst;
  inst.load("../../instances/exemplo_light.gcc");
  LightEdgePreprocessor pre;
  std::set<int> fixedOnes, fixedZeros;
  pre.fixVariables(inst, fixedOnes, fixedZeros);
  assert(pre.ones.size() == 2);
  assert(pre.zeros.size() == 0);
}


void bridgeTest() {
  Instance inst;
  inst.load("../../instances/exemplo_bridges.gcc");
  BridgePreprocessor pre;
  std::set<int> fixedOnes, fixedZeros;
  pre.fixVariables(inst, fixedOnes, fixedZeros);
  assert(pre.ones.size() == 2);
  assert(pre.zeros.size() == 1);
}

int main(int argc, char ** argv) {
  lightEdgeTest();
  bridgeTest();
}
