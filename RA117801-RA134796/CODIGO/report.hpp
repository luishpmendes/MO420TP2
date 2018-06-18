//output information and extra info used to generate graphs and tables
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
  int totalOCICuts;
  
  double firstDualBound;
  double rootDualBound;

  int numNodes;
  int bestPrimalNode;

  int bestPrimalBound;
  double bestDualBound;

  double totalTime;
  std::string toString(const Params & params) const{
    std::string ret = "";
    
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

  Info() {
    fixedOnes = fixedZeros = addedConflicts = removedConflicts = 0;

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

