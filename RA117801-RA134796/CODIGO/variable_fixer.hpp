//classes that fix variables : preprocessors and variablefixers

#pragma once
#include <set>
#include <string>
#include <utility>
#include <string>
#include <vector>
#include "instance.hpp"
#include "constants.hpp"

class VariableFixer {
  public:
  std::string name;
  std::set<int> ones, zeros;
  std::set<std::pair<int,int>> conflicts;

  virtual void fixVariables(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros) {

  }
};


//fix light edges that doesn't have conflicts (run before solving)
class LightEdgePreprocessor : public VariableFixer {
  public:
  LightEdgePreprocessor() {
    name = "LightEdgePre";
  }

  void fixVariables(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros);
};

//fix necessary edges to keep connectivity (bridges) (run before solving)
class BridgePreprocessor : public VariableFixer{
  std::vector<int> dfsNum, dfsLow;
  std::vector<std::set<std::pair<int, int>>> adjGraph; //set allows easy removal (vertex,
  //edge_id)
  int dfsCounter;
  std::vector<int> curBridges; // bridges current iteration (to be fixed)
  std::vector<std::pair<int,int>> curConflicts; // conflicts to be added
  void dfs(int u, int parent, const std::set<int> & czeros); //bridge detection basad on Competitive Programming 3 code
  public:
  BridgePreprocessor() {
    name = "BridgePre";
  }

  bool identifyBridges(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros);
  bool identifyInfeasibleEdges(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros);
  bool identifyConflicts(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros);

  void fixVariables(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros);
};


