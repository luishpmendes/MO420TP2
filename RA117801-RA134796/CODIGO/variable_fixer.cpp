#include "lagrangean_solver.hpp"
#include "variable_fixer.hpp"
#include "mergefind.hpp"

void LightEdgePreprocessor::fixVariables(void *solver) {
  //local conflcits graphs
  LagrangeanSolver *lsolver= (LagrangeanSolver*)solver;
  std::vector<std::set<int>> conflictsGraph(lsolver->inst->edges.size());
  std::vector<std::pair<int,int>> wid;
  std::set<int> active;
  for(int e = 0; e < lsolver->inst->edges.size();e++) {
    if(lsolver->fixedOnes.count(e)) {
      wid.push_back(std::pair<int, int>(-1, e));
    }
    else if(lsolver->fixedZeros.count(e)) {
      continue;
    }
    else {
      wid.push_back(std::pair<int, int>(lsolver->inst->cost[e], e));
    }
    active.insert(e);
  }
  for(int e : active) {
    for(int f : lsolver->inst->conflictsGraph[e]) {
      if(active.count(f)) 
        conflictsGraph[e].insert(f);
    }
  }
  
  //sort by weight
  std::sort(wid.begin(), wid.end());
  bool change =  true;
  int maxFixedValue=-1;
  int maxFixedI = -1;
  MergeFind mergeFind(lsolver->inst->size);
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
        if(mergeFind.merge(lsolver->inst->edges[wid[i].second].u,
            lsolver->inst->edges[wid[i].second].v)) {
          if(lsolver->fixedOnes.count(wid[i].second) == 0)
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
      if(mergeFind.find(lsolver->inst->edges[e].u) == 
          mergeFind.find(lsolver->inst->edges[e].v)) {
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


bool BridgePreprocessor::identifyBridges(void *solver) {
    bool change = false;
    LagrangeanSolver *lsolver= (LagrangeanSolver*)solver;
    dfsNum.assign(adjGraph.size(), -1);
    dfsLow.assign(adjGraph.size(), 0);
    dfsCounter = 0;
    curBridges.clear();
    dfs(0, -1, std::set<int>());

    if(dfsCounter != adjGraph.size()) {
      //não-conexo
    }
    for(int b : curBridges) {
      if(ones.count(b) == 0 && lsolver->fixedOnes.count(b) == 0) {
        change = true;
        ones.insert(b);
        for(int f : lsolver->inst->conflictsGraph[b]) {
          adjGraph[lsolver->inst->edges[f].u].erase(
            std::pair<int,int>(lsolver->inst->edges[f].v, f));
          adjGraph[lsolver->inst->edges[f].v].erase(
            std::pair<int,int>(lsolver->inst->edges[f].u, f));
          zeros.insert(f);
        }
      }
    }
  return change;
}

bool BridgePreprocessor::identifyInfeasibleEdges(void *solver) {
  LagrangeanSolver *lsolver= (LagrangeanSolver*)solver;
  bool change = false;
  for(int e = 0; e < lsolver->inst->edges.size(); e++) {
    if(ones.count(e) || lsolver->fixedOnes.count(e) ||
       zeros.count(e) || lsolver->fixedZeros.count(e) ||
       lsolver->inst->conflictsGraph[e].size() == 0)
      continue;
    std::set<int> econf;
    for(int f : lsolver->inst->conflictsGraph[e]) {
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
        adjGraph[lsolver->inst->edges[e].u].erase(
          std::pair<int,int>(lsolver->inst->edges[e].v, e));
        adjGraph[lsolver->inst->edges[e].v].erase(
          std::pair<int,int>(lsolver->inst->edges[e].u, e));
        zeros.insert(e);
        break;
      }
      int numFixedBefore = econf.size();
      for(int b : curBridges) {
        for(int f : lsolver->inst->conflictsGraph[b]) {
          econf.insert(f);
        }
      }
      if(econf.size() == numFixedBefore)
        break;
    }
  }
  return change;
}
bool BridgePreprocessor::identifyConflicts(void *solver) {
  curConflicts.clear();
  return false;
}
void BridgePreprocessor::fixVariables(void *solver) {
  LagrangeanSolver *lsolver= (LagrangeanSolver*)solver;
  zeros.clear();
  ones.clear();
  //construct adjGraph
  adjGraph.clear();
  adjGraph.resize(lsolver->inst->size);
  for(int e = 0; e < lsolver->inst->edges.size(); e++) {
    if(lsolver->fixedZeros.count(e) == 0) {
      adjGraph[lsolver->inst->edges[e].u].insert(
        std::pair<int,int>(lsolver->inst->edges[e].v, e));
      adjGraph[lsolver->inst->edges[e].v].insert(
        std::pair<int,int>(lsolver->inst->edges[e].u, e));
    }
  }
  bool change = true;
  while(change) {
    change = false;
    if(identifyBridges(solver))
      change = true;
    if(identifyInfeasibleEdges(solver))
      change = true; 
  }
};



