#pragma  once
#include <vector>
class MergeFind { //unionfind ds, but union is a reserved word :(
  public:
  std::vector<int> g;
  std::vector<int> rank;

  MergeFind(int n) {
    g.resize(n);
    rank.resize(n);
    for(int i = 0; i < n; i++) {
      g[i] = -1;
      rank[i] = 1;
    }
  }

  int find(int v){
    while(g[v] != -1) {
      v = g[v];
    }
    return v;
  } 

  bool merge(int u, int v) {
    int lu = find(u);
    int lv = find(v);
    
    if(lu == lv)
      return false;

    if(rank[lu] > rank[lv]){
      g[lv] = lu;
    }
    else if(rank[lu] < rank[lv]){
      g[lu] = lv;
    }
    else {
      g[lu] = lv;
      rank[lv]++;
    }
    return true;
  }
};

