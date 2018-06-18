//tries to make lagrangean primal solutions viable
#pragma once
#include <string>
#include "constants.hpp"
#include "solution.hpp"

class SolutionAdapter {
  public:
  std::string name;
  bool viable;
  Solution solution;
  double primalBound;
  virtual void adapt(void* solver);
};

//builds the solution from x vector if viable, otherwise, calculates a
// primalbound heavily penalyzing conflicts (+totalWeight*conflicts)
class ViableSolutionAdapter : public SolutionAdapter {
  public:
  ViableSolutionAdapter() {
    name = "ViableSA";
  }
  void adapt(void *solver);
};


//tries to exchange conflicts edge for other viable edges, choices are made at
// random
class SwapConflictsSolutionAdapter : public SolutionAdapter {
  public:
  int failureTolerance; // number of times it can fail to make a change
  SwapConflictsSolutionAdapter(int tol=20) {
    failureTolerance = tol;
    name = "SwapConflictsSA";
  }
  void adapt(void *solver);
};


//pick a random edge at random (based on weight and number of conflicts) and
// tries to replace it.
class RandomFlipsSolutionAdapter : public SolutionAdapter {
  public:
  int failureTolerance; // number of times it can fail to make a change
  int rep;
  std::vector<int> curNumViolatedPerEdge;
  int curNumViolated;
  std::set<int> curX;

  bool curViable;
  Solution curSolution;
  
  RandomFlipsSolutionAdapter(int tol=20, int _rep=1) {
    failureTolerance = tol;
    rep = _rep;
    name = "SwapConflictsSA";
  }

  void flip(void *solver);
  void buildConflictGraph(void *solver);
  void adapt(void *solver);
};
