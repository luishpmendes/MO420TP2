#include "variable_fixer.hpp"
#include "mergefind.hpp"
#include <algorithm> 

void LightEdgePreprocessor::fixVariables(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros) {
  //local conflcits graphs
  std::vector<std::set<int>> conflictsGraph(inst.edges.size());
  std::vector<std::pair<int,int>> wid;
  std::set<int> active;
  for(int e = 0; e < inst.edges.size();e++) {
    if(fixedOnes.count(e)) {
      wid.push_back(std::pair<int, int>(-1, e));
    }
    else if(fixedZeros.count(e)) {
      continue;
    }
    else {
      wid.push_back(std::pair<int, int>(inst.cost[e], e));
    }
    active.insert(e);
  }
  for(int e : active) {
    for(int f : inst.conflictsGraph[e]) {
      if(active.count(f)) 
        conflictsGraph[e].insert(f);
    }
  }
  
  //sort by weight
  std::sort(wid.begin(), wid.end());
  bool change =  true;
  int maxFixedValue=-1;
  int maxFixedI = -1;
  MergeFind mergeFind(inst.size);
  ones.clear();
  zeros.clear();

  //fix light edges
  while(change) {
    int i = 0;
    int maxValue = INF;
    change = false;
    while(i < wid.size() && wid[i].first <= maxValue) {
      //skips not active
      if(active.count(wid[i].second) == 0) {
        i++;
        continue;
      }
      if(conflictsGraph[wid[i].second].size() == 0) { // no conflict 
        if(mergeFind.merge(inst.edges[wid[i].second].u,
            inst.edges[wid[i].second].v)) {
          if(fixedOnes.count(wid[i].second) == 0)
            ones.insert(wid[i].second);
        }
        else { //cycle
          zeros.insert(wid[i].second);
          for(int f : conflictsGraph[wid[i].second]) {
            conflictsGraph[f].erase(wid[i].second);
          }
        }
        active.erase(wid[i].second);
        maxFixedI++;
        maxFixedValue = wid[i].first;
        change = true;
      }
      else {
        maxValue = wid[i].first;
      }
      i++;
    }
    //updates conflicts
    std::vector<int> temp;
    for(int e : active) {
      if(mergeFind.find(inst.edges[e].u) == 
          mergeFind.find(inst.edges[e].v)) {
        zeros.insert(e);
        for(int f : conflictsGraph[e]) {
          conflictsGraph[f].erase(e);
        }
        temp.push_back(e);
      }
    }
    for(int e : temp)
      active.erase(e);
  }
}

//depth first search to find bridges
void BridgePreprocessor::dfs(int u, int parent, const std::set<int> & czeros) {
  dfsLow[u] = dfsNum[u] = dfsCounter++;
  for(std::pair<int,int> p : adjGraph[u]) {
    if(czeros.count(p.second) != 0)
      continue;
    if(dfsNum[p.first] == -1) {//not seem
      dfs(p.first,  u, czeros);
      if(dfsLow[p.first] > dfsNum[u]) { //bridge
        curBridges.push_back(p.second);
      }
      dfsLow[u] = std::min(dfsLow[u], dfsLow[p.first]);
    }
    else if(p.first != parent) {// back edge 
      dfsLow[u] = std::min(dfsLow[u], dfsNum[p.first]);
    }
  }
}


bool BridgePreprocessor::identifyBridges(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros) {
    bool change = false;
    dfsNum.assign(adjGraph.size(), -1);
    dfsLow.assign(adjGraph.size(), 0);
    dfsCounter = 0;
    curBridges.clear();
    dfs(0, -1, std::set<int>());

    if(dfsCounter != adjGraph.size()) {
      //não-conexo
    }
    for(int b : curBridges) {
      if(ones.count(b) == 0 && fixedOnes.count(b) == 0) {
        change = true;
        ones.insert(b);
        for(int f : inst.conflictsGraph[b]) {
          adjGraph[inst.edges[f].u].erase(
            std::pair<int,int>(inst.edges[f].v, f));
          adjGraph[inst.edges[f].v].erase(
            std::pair<int,int>(inst.edges[f].u, f));
          zeros.insert(f);
        }
      }
    }
  return change;
}

bool BridgePreprocessor::identifyInfeasibleEdges(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros) {
  bool change = false;
  for(int e = 0; e < inst.edges.size(); e++) {
    if(ones.count(e) || fixedOnes.count(e) ||
       zeros.count(e) || fixedZeros.count(e) ||
       inst.conflictsGraph[e].size() == 0)
      continue;
    std::set<int> econf;
    for(int f : inst.conflictsGraph[e]) {
      econf.insert(f);
    }
    while(true) {
      dfsNum.assign(adjGraph.size(), -1);
      dfsLow.assign(adjGraph.size(), 0);
      dfsCounter = 0;
      curBridges.clear();
      dfs(0, -1, econf);
      if(dfsCounter != adjGraph.size()) {
        change = true;
        adjGraph[inst.edges[e].u].erase(
          std::pair<int,int>(inst.edges[e].v, e));
        adjGraph[inst.edges[e].v].erase(
          std::pair<int,int>(inst.edges[e].u, e));
        zeros.insert(e);
        break;
      }
      int numFixedBefore = econf.size();
      for(int b : curBridges) {
        for(int f : inst.conflictsGraph[b]) {
          econf.insert(f);
        }
      }
      if(econf.size() == numFixedBefore)
        break;
    }
  }
  return change;
}
bool BridgePreprocessor::identifyConflicts(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros) {
  curConflicts.clear();
  return false;
}
void BridgePreprocessor::fixVariables(const Instance & inst,
    const std::set<int> &fixedOnes, const std::set<int> &fixedZeros) {
  zeros.clear();
  ones.clear();
  //construct adjGraph
  adjGraph.clear();
  adjGraph.resize(inst.size);
  for(int e = 0; e < inst.edges.size(); e++) {
    if(fixedZeros.count(e) == 0) {
      adjGraph[inst.edges[e].u].insert(
        std::pair<int,int>(inst.edges[e].v, e));
      adjGraph[inst.edges[e].v].insert(
        std::pair<int,int>(inst.edges[e].u, e));
    }
  }
  bool change = true;
  while(change) {
    change = false;
    if(identifyBridges(inst, fixedOnes, fixedZeros))
      change = true;
    if(identifyInfeasibleEdges(inst, fixedOnes, fixedZeros))
      change = true; 
  }
};



