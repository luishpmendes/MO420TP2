//represents solutions
#pragma once
#include <utility>
#include <cmath> 
#include <vector>
#include "instance.hpp"
#include "mergefind.hpp"
#include "constants.hpp"

class Solution {
  public:
  std::vector<double> vars;
  double value;

  void clear() {
    vars.clear();
    value = 0;
  }

  void fromIncidence(const std::vector<int> & x, Instance * inst) {
    vars.clear();
    value = 0;
    for(int i = 0; i < x.size(); i++) {
      if(x[i] == 1) {
        value += inst->cost[i];
        vars.push_back(i);
      }
    } 
  }
  bool validate(Instance *inst) {
    if(vars.size() != inst->size-1)
      return false;
    double calcValue = 0;
    MergeFind mergeFind(inst->size);
    for(int e : vars) {
      if(!mergeFind.merge(inst->edges[e].u, inst->edges[e].v))
        return false;
      calcValue += inst->cost[e];
    }
    if(std::fabs(calcValue-value) >= 1)
      return false;
    return true;
  }

  std::string toString(const Instance & inst, bool printValue=true,
    double threshold=0.1) const {
    std::string ret;
    if(printValue)
      ret += std::to_string(value)+"\n";
    for(int i = 0; i < vars.size(); i++) {
      if(vars[i] > threshold) {
        ret += inst.edges[vars[i]].toString();
        if(printValue) {
          ret += " "+std::to_string(vars[i]);
        }
        ret += "\n";
      }
    }
    return ret;
  }
};
