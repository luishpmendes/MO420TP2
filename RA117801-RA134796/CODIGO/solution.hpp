//represents solutions
#pragma once
#include <utility>
#include <cmath> 
#include <vector>
#include "instance.hpp"
#include "mergefind.hpp"
#include "constants.hpp"
#include "global.hpp"

#include "ilcplex/ilocplex.h"
class Solution {
  public:
  std::vector<int> vars;
  double value;

  bool invalid;

  void clear() {
    vars.clear();
    value = 0;
    invalid = false;
  }


  void fromIloNumArray(const IloNumArray & vals, const Instance & inst) {
    vars.clear();
    value = 0;
    for(int i = 0; i < inst.edges.size(); i++) {
      if(vals[i] > 0.5) {
         value += inst.cost[i];
         vars.push_back(i);
      }
    }
  }

  void fromIncidence(const std::vector<int> & x, const Instance & inst) {
    vars.clear();
    value = 0;
    for(int i = 0; i < x.size(); i++) {
      if(x[i] == 1) {
        value += inst.cost[i];
        vars.push_back(i);
      }
    } 
  }
  bool validate(const Instance & inst) {
    invalid = true;
    if(vars.size() != inst.size-1) {
      return false;
    }
    double calcValue = 0;
    MergeFind mergeFind(inst.size);
    for(int e : vars) {
      if(!mergeFind.merge(inst.edges[e].u, inst.edges[e].v))
        return false;
      calcValue += inst.cost[e];
    }
    if(std::fabs(calcValue-value) >= 1)
      return false;
    
    invalid = false;
    return true;
  }

  std::string toString(const Instance & inst, bool printValue=false,
    double threshold=0.1) const {
    std::string ret;
    if(invalid) {
      if(printValue)
        ret += std::to_string(INF)+"\n";
      for(int i = 0; i <  inst.size-1; i++)
        ret += "-1 -1\n";
      return ret;
    }
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

  void write(const Instance & inst, std::string filename) {
    std::ofstream file;
    file.open(filename);
    file << toString(inst);
    file.close();
  }

};
