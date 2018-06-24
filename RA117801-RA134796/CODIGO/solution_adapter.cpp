#include "solution_adapter.hpp"
#include "lagrangean_solver.hpp"

//helper funcion
int randomPick(const std::vector<double> & w) {
  double wTot = 0;
  for(double v : w)
    wTot += v;
  
  int pick = w.size()-1;
  double randV = (rand()/(RAND_MAX+1.0))*wTot;
  for(int i = 0; i < w.size(); i++) {
    if(randV < w[i]) {
      pick = i;
      break;
    }
  }
  return pick;
}

void SolutionAdapter::adapt(void* solver) {
  LagrangeanSolver *lsolver = (LagrangeanSolver*)solver;
  viable = false;
  solution.value = (lsolver->inst->maxEdgeWeight+1)*(lsolver->inst->size-1);
}


void ViableSolutionAdapter::adapt(void *solver) {
  LagrangeanSolver *lsolver = (LagrangeanSolver*)solver;
  std::set<int> tightConflicts;
  int violated = 0;

  viable = true;
  solution.value = 0;
  solution.vars.clear();
  double totalWeight =0;

  //finds violated conflicts
  for(int i = 0; i < lsolver->x.size(); i++) {
    if(lsolver->fixedZeros.count(i) >0)
      continue;

    totalWeight += lsolver->inst->cost[i];
    if(lsolver->x[i] == 1) {
      solution.vars.push_back(i);
      solution.value += lsolver->inst->cost[i];
      for(int conflict : lsolver->inst->conflictsByEdge[i]) {
        if(tightConflicts.count(conflict) > 0)
          violated++;
        else 
          tightConflicts.insert(conflict);
      }
    }
  }
  if(violated >  0) {
    viable = false;
    solution.value += violated*totalWeight;
  }
}

//abandoned before optimization
void SwapConflictsSolutionAdapter::adapt(void  *solver) {
  LagrangeanSolver *lsolver = (LagrangeanSolver*)solver;

  //construct conflicts
  std::set<int> tightConflicts, violatedConflicts;
  std::set<int> x;
  double totalWeight = 0;
  for(int i = 0; i < lsolver->x.size(); i++) {
    if(lsolver->x[i] == 1) {
      x.insert(i);
    }
  }
  for(int e: x) {
    for(int conflict : lsolver->inst->conflictsByEdge[e]) {
      if(tightConflicts.count(conflict) == 0) {
        tightConflicts.insert(conflict);
      }
      else {
        violatedConflicts.insert(conflict);
      }
    }
    
  }
  int lastE; //used to retrieve an erase x when conflicts are not eliminated
  while(violatedConflicts.size() > 0)  {
    int eid = rand()%(violatedConflicts.size());
    auto eit = violatedConflicts.begin();
    advance(eit, eid);
    int conflict = *(eit); //conflicts being solved

    int e; //e must be removed
    if(lsolver->fixedOnes.count(lsolver->inst->conflicts[conflict].first) &&
       lsolver->fixedOnes.count(lsolver->inst->conflicts[conflict].second)) {
      throw "Invalid fixing!!!";
      break; //cannot fix conflict!!!
    }
    else if(lsolver->fixedOnes.count(lsolver->inst->conflicts[conflict].first)) {
      e = lsolver->inst->conflicts[conflict].second;
    }
    else if(lsolver->fixedOnes.count(lsolver->inst->conflicts[conflict].second)) {
      e = lsolver->inst->conflicts[conflict].first;
    } else {
      if(rand()%2 == 0)
        e = lsolver->inst->conflicts[conflict].first;
      else
        e = lsolver->inst->conflicts[conflict].second;
    }
    lastE = e;
    //finds two components left after  lastE is removed
    x.erase(e);
    MergeFind mergeFind(lsolver->inst->size);
    for(int f : x) {
      if(f == e)
        continue;
      mergeFind.merge(lsolver->inst->edges[f].u, lsolver->inst->edges[f].v);
    }

    ///finds candidate edges and their weight
    std::vector<int> candidates;
    double totalWeight=0; //candidateweight = 1/edgeweight
    for(int i = 0; i < lsolver->inst->edges.size();i++) {
      //does i belongs to the cut?
      int invalid = false;
      if(lsolver->fixedZeros.count(i) > 0)
        invalid =  true;
      if(!invalid && i !=  e &&  
          (mergeFind.find(lsolver->inst->edges[i].u) !=
          mergeFind.find(lsolver->inst->edges[i].v))) {
        for(int g : lsolver->inst->conflictsGraph[i]) {
          if(x.count(g) > 0) {
            invalid = true;
            continue;
          }
        }
        if(!invalid) {
          candidates.push_back(i);
          totalWeight += 1.0/lsolver->inst->cost[i];
        }
      }
    }
    if(candidates.size() == 0) {
      break; // can't be fixed by this heuristic
    }
    //pick at random
    double randV = (rand()/(RAND_MAX+1.0))*totalWeight;
    int newE=candidates[candidates.size()-1];
    for(int c: candidates) {
      if(randV < 1.0/lsolver->inst->cost[c]) {
        newE = c;
        break;
      }
      randV -= 1.0/lsolver->inst->cost[c];
    }
    //fix conflicts
    std::vector<int> removeConflicts;
    for(int c : violatedConflicts) {
      if(lsolver->inst->conflicts[c].first == lastE ||
          lsolver->inst->conflicts[c].second == lastE)
        removeConflicts.push_back(c);
    }
    for(int c : removeConflicts)
      violatedConflicts.erase(c);
    x.insert(newE);
    for(int conflictNewE : lsolver->inst->conflictsByEdge[newE]) {
      tightConflicts.insert(conflictNewE);
    }
  }
  //constructs solition
  if(violatedConflicts.size() == 0) {
    viable = true;
    solution.value = 0;
    solution.vars.clear();
    for(int e : x) {
      solution.vars.push_back(e);
      solution.value += lsolver->inst->cost[e];
    }
  }
  else {
    viable = false;
    for(int e : x) {
      solution.value += lsolver->inst->cost[e];
    }
    solution.value += lsolver->inst->cost[lastE];
    solution.value += (lsolver->inst->maxEdgeWeight+1)*(lsolver->inst->size-1);
  }
}


void RandomFlipsSolutionAdapter::buildConflictGraph(void *solver) {
  LagrangeanSolver *lsolver = (LagrangeanSolver*)solver;
  //construct conflicts
  
  double totalWeight = 0;
  curX.clear();
  for(int i = 0; i < lsolver->x.size(); i++) {
    if(lsolver->x[i] == 1) {
      curX.insert(i);
    }
  }
  //findViolated
  curNumViolated=0;
  curNumViolatedPerEdge.assign(lsolver->inst->edges.size(), 0);

  for(int e :curX) {
    for(int f: curX) {
      if(lsolver->inst->conflictsSet.count(pii(e,f)) > 0) {
        curNumViolated++;
        curNumViolatedPerEdge[e]++;
        curNumViolatedPerEdge[f]++;
      }
    }
  }
}

void RandomFlipsSolutionAdapter::flip(void  *solver) {
  LagrangeanSolver *lsolver = (LagrangeanSolver*)solver;
  std::set<int> x = curX;
  std::vector<int> numViolated = curNumViolatedPerEdge;
  int numViolatedConflicts = curNumViolated;

  int conseqFailures = 0;
  std::vector<int> candOut, candIn;
  std::vector<double> wOut, wIn;
  candOut.reserve(x.size());
  wOut.reserve(x.size());
  while(conseqFailures < failureTolerance) {
    //get a random edge to exit
    candOut.resize(0);
    wOut.resize(0);
    for(int e : x) {
      if(lsolver->fixedOnes.count(e) == 0) {
        if(numViolatedConflicts > 0 && numViolated[e] == 0)
          continue;
        candOut.push_back(e);
        double candOutCost = lsolver->inst->cost[e];
        candOutCost += (lsolver->inst->maxEdgeWeight+1)*numViolated[e];
        wOut.push_back(candOutCost);
      }
    }
    if(candOut.size() == 0) {
      break;
    }
    int pickOut = randomPick(wOut);
    int eOut = candOut[pickOut];
    int wEOut = wOut[pickOut];
    x.erase(eOut);

    //finds connected components after  removing eOut
    MergeFind mergeFind(lsolver->inst->size);
    for(int f : x) {
      mergeFind.merge(lsolver->inst->edges[f].u, lsolver->inst->edges[f].v);
    }
    
    //finds candidate edges to be added
    candIn.resize(0);
    wIn.resize(0);
    for(int e = 0; e <  lsolver->inst->edges.size(); e++) {
      Edge edge = lsolver->inst->edges[e];
      if(mergeFind.find(edge.u) != mergeFind.find(edge.v) &&
          lsolver->fixedZeros.count(e) == 0 && e != eOut) {
        int valid=true;
        //needs to be lighter or have less conflicts
        for(int f : lsolver->inst->conflictsGraph[e]) {
          if(x.count(f) > 0) {
            valid = false;
            break;
          }
        }
        if(!valid)
          continue;
        if(lsolver->inst->cost[e] > wEOut)
          continue;
        candIn.push_back(e);
        wIn.push_back(1.0/(2.0+lsolver->inst->cost[e]));
        
      }
    }
    if(candIn.size() == 0) {
      x.insert(eOut);
      conseqFailures++;
      continue;
    }

    //insert new edge
    int eIn = candIn[randomPick(wIn)];
    for(int f : lsolver->inst->conflictsGraph[eOut]) {
      if(x.count(f) > 0) {
        numViolated[f]--;
        numViolatedConflicts--;
      }
    }
    x.insert(eIn);
  }

  //builds solution
  curSolution.clear();
  curSolution.value = 0;
  for(int e : x) {
    curSolution.vars.push_back(e);
    curSolution.value += lsolver->inst->cost[e];
  }
  curSolution.value += numViolatedConflicts*(lsolver->inst->maxEdgeWeight+1)*
      (lsolver->inst->size-1);
  curViable = (numViolatedConflicts == 0);
}


void RandomFlipsSolutionAdapter::adapt(void *solver) {
  buildConflictGraph(solver);
  viable = false;
  solution.value = INF;
  for(int i = 0; i < rep; i++) {
    flip(solver);
    if((!viable && curViable)||curSolution.value < solution.value) {
      solution=curSolution;
      viable=curViable;
    }
  }
}
