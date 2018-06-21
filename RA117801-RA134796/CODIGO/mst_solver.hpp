//implements MST solvers
#pragma once
#include <vector>
#include <set>
#include <utility>
#include <algorithm>
#include "mergefind.hpp"
#include "instance.hpp"
class MSTSolver {
  public:
  bool viable;
  virtual double solve(Instance *inst, const std::vector<double> & w,
    const std::set<int> & fixedOnes, const std::set<int> & fixedZeros,
    std::vector<int> &x) = 0;//x is output
};


class KruskalMSTSolver : public MSTSolver{
  public:
  double solve(Instance *inst, const std::vector<double> & w,
    const std::set<int> & fixedOnes, const std::set<int> & fixedZeros,
    std::vector<int> &x) {

      int insertions = 0;
      double value = 0;

      std::vector<std::pair<double, int>> wid;
      wid.resize(w.size());
      MergeFind mergeFind(inst->size);
      //init
      for(int i = 0; i <w.size(); i++) {
        wid[i].first =  w[i];
        wid[i].second = i;
        x[i] = 0;
      }
      //fix ones
      for(int one: fixedOnes) {
        insertions++;
        x[one]= 1;
        value += w[one];
        mergeFind.merge(inst->edges[one].u, inst->edges[one].v);
      }

      std::sort(wid.begin(), wid.end());
      //remaining edges
      for(int i = 0;i < wid.size(); i++) {
        if(fixedZeros.count(wid[i].second) > 0)
          continue;
        if(mergeFind.merge(
            inst->edges[wid[i].second].u, inst->edges[wid[i].second].v)) {
          value +=wid[i].first;
          x[wid[i].second] = 1;
          insertions++;
        }
      }
      viable = (insertions == inst->size-1);
      return value;
  }
};
