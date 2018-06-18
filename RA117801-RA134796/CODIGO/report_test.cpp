#include "report.hpp"
#include <cassert>
#include <iostream>

int main(int argc, char ** argv) {
  Params params;
  Instance inst;
  inst.load("../../instances/exemplo.gcc");
  Report rep;
  std::cout << rep.toString(params);
}
