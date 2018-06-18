//classes that fix variables : preprocessors and variablefixers

#pragma once
#include <set>
#include <string>

class VariableFixer {
  public:
  std::string name;
  std::set<int> ones, zeros;
  std::set<std::pair<int,int>> conflicts;

  virtual void fixVariables(void *solver) {

  }
};


//fix light edges that doesn't have conflicts (run before solving)
class LightEdgePreprocessor : public VariableFixer {
  public:
  LightEdgePreprocessor() {
    name = "LightEdgePre";
  }
  void fixVariables(void *solver);
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

  bool identifyBridges(void *solver);
  bool identifyInfeasibleEdges(void *solver);
  bool identifyConflicts(void *solver);
  void fixVariables(void *solver);
};


