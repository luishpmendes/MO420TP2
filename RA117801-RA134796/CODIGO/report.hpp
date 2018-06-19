//output information and extra info used to generate graphs and table
#pragma once
#include <utility>
#include <string>
#include <vector>
#include "solution.hpp"
#include "params.hpp"

#include <fstream>

template <class T>
std::string to_string(std::vector<T> vec) {
  std::string ret = "";
  for(int i = 0; i < vec.size();i++) {
    ret += std::to_string(vec[i])+" ";
  }
  return ret;
}

class Report {
  public:
  int totalSTCuts;
  int totalOCCuts;
  
  double firstDualBound;
  double rootDualBound;

  int numNodes;
  int bestPrimalNode;

  int bestPrimalBound;
  double bestDualBound;

  double totalTime;

  Report() {
    totalSTCuts = 0;
    totalOCCuts = 0;

    firstDualBound = -INF;
    rootDualBound  = -INF;

    numNodes = 0;
    bestPrimalNode = -1;
  
    bestPrimalBound = INF;
    bestDualBound = -INF;

    totalTime = 0;
  }

  std::string toString(const Params & params) const{
    std::string ret = "";
    //ret += params.toString() + "\n";
    ret += std::to_string(totalSTCuts) + "\n";
    ret += std::to_string(totalOCCuts) + "\n";

    ret += std::to_string(firstDualBound) + "\n";
    ret += std::to_string(rootDualBound) + "\n";

    ret += std::to_string(numNodes) + "\n";
    ret += std::to_string(bestPrimalNode) + "\n";

    
    ret += std::to_string(bestPrimalBound) + "\n";
    ret += std::to_string(bestDualBound) + "\n";

    ret += std::to_string(totalTime) + "\n";
    return ret;
  }

  void write(const Params & params, std::string filename) {
    std::ofstream file;
    file.open(filename);
    file << toString(params);
    file.close();
  }
};

//extra info
class Info {
  public:
  int fixedOnes, fixedZeros;
  int addedConflicts, removedConflicts;

  //do not print
  int iterSep;
  bool firstNode;
  Info() {
    fixedOnes = fixedZeros = addedConflicts = removedConflicts = 0;
    iterSep = 0;
    firstNode = true;
  }
  std::string toString(){
    std::string ret =  "";
    ret += std::to_string(fixedOnes)+"\n";
    ret += std::to_string(fixedZeros)+"\n";
    ret += std::to_string(addedConflicts)+"\n";
    ret += std::to_string(removedConflicts)+"\n";
    return ret;
  }

  void write(std::string filename) {
    std::ofstream file;
    file.open(filename);
    file << toString();
    file.close();
  }
};

