//representation of instances and related information (edges, conflicts)
#pragma once
#include <cstdio>
#include <iostream>
#include <fstream>
#include <map>
#include <utility>
#include <vector>
#include <string>
#include <set>

typedef std:: pair<int,int> pii;
class Edge {
  public:
  int u, v;
  Edge(int _u, int _v) { //corrige ordem
    u = std::min(_u, _v);
    v = std::max(_u, _v);
  }

  bool operator < (const Edge & e) const {
    if(u < e.u)
      return true;
    if(u == e.u)
      return v < e.v;
    return false;
  }

  bool operator == (const Edge & e) const {
    return (u == e.u && v == e.v);
  }

  std::string toString() const {
    return std::to_string(u)+" "+std::to_string(v);
  }
};

class ConflictGraph {
  public:
  std::vector<pii> conflictsList;
  std::vector<std::set<int>> conflictsByEdge;
  std::vector<std::set<int>> conflictsAdj;

  void resize(int m) {
    conflictsByEdge.resize(m);
    conflictsAdj.resize(m);
  }

  int add(int u, int v) {
    int c = conflictsList.size();
    conflictsList.push_back(pii(u,v));
    conflictsByEdge[u].insert(c);
    conflictsByEdge[v].insert(c);
    conflictsAdj[u].insert(v);
    conflictsAdj[v].insert(u);

    return c;
  }

  void remove(int c) {
    int u = conflictsList[c].first;
    int v = conflictsList[c].second;
    conflictsByEdge[u].erase(c);
    conflictsByEdge[v].erase(c);
    conflictsAdj[u].erase(v);
    conflictsAdj[v].erase(u);
  }
};

class Instance {
  public:
  int size;
  std::vector<Edge> edges;
  std::vector<std::vector<int>> adjList; //lists of edge ids

  std::map<Edge, int> edge2id; //maps edges to ids
  std::vector<double> cost;
  int minWeightEdgeId;
  int maxEdgeWeight;

  ConflictGraph conflictGraph;
  std::vector<std::pair<int,int>> conflicts; // pares (por id) bloqueados
  std::set<std::pair<int,int>> conflictsSet;
  std::vector<std::vector<int>> conflictsByEdge;//edge id -> conflic edge ids
  std::vector<std::vector<int>> conflictsGraph;//edge id -> edge id
  
  std::string name;

  int getOtherVertice(int u, int e) {
    if (u == edges[e].u) {
        return edges[e].v;
    }

    return edges[e].u;
  }

  void load(const char * filename) {
    std::ifstream fs;

    fs.open(filename);
    if(!fs.is_open())
      throw "File not found";
    
    std::string buffer;
    //skips comments
    while (getline(fs, buffer)) {
      if(buffer[0] != '#')
        break;
    }
    name = std::string(buffer);
    int m, p;
    fs >> size >> m >> p;
    
    //edges
    edges.reserve(m);
    adjList.resize(size);
    maxEdgeWeight = 0;
    for(int i = 0; i < m;i++) {
      int u, v, c;
      int id = edges.size();
      fs >> u >> v >> c;
      Edge e(u,v);
      edges.push_back(e);
      cost.push_back(c);
      edge2id[e] = id;

      adjList[u].push_back(id);
      adjList[v].push_back(id);
      maxEdgeWeight = std::max(maxEdgeWeight, c);
    }

    //conflicts
    conflicts.reserve(p);
    conflictsByEdge.resize(m);
    conflictsGraph.resize(m);
    conflictsSet.clear();

    conflictGraph.resize(m);
    for(int i = 0; i < p; i++) {
      int tu, tv;

      int eId, fId;
      fs >> tu >> tv;
      eId = edge2id[Edge(tu, tv)];

      fs >> tu >> tv;
      fId = edge2id[Edge(tu, tv)];
      
      conflictGraph.add(eId, fId);
      
      conflicts.push_back(std::pair<int,int>(eId, fId));
      conflictsSet.insert(std::pair<int,int>(eId, fId));
      conflictsByEdge[eId].push_back(conflicts.size()-1);
      conflictsByEdge[fId].push_back(conflicts.size()-1);
      conflictsGraph[eId].push_back(fId);
      conflictsGraph[fId].push_back(eId);
    }

    minWeightEdgeId = -1;
    for (int i = 0 ; i < (int) edges.size(); i++){
      if (conflictsByEdge[i].size() == 0)
        continue;
        
      if(minWeightEdgeId == -1 || cost[minWeightEdgeId] > cost[i])
        minWeightEdgeId = i;
    }
  }
};

