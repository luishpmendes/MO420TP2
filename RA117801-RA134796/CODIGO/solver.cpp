#include <set>
#include <utility>
#include <vector>

#include "cuts.hpp"
#include "global.hpp"
#include "mc_solver.hpp"
#include "primalheur.hpp"
#include "solver.hpp"

#include "ilcplex/ilocplex.h"

void BNCSolver::createBaseModel() {
  _model = IloModel(_env);
  _x = IloBoolVarArray(_env, _inst.edges.size());

  IloExpr treeConst(_env);
  for(int i = 0; i < _inst.edges.size(); i++) {
    treeConst += _x[i];
  }
  _model.add(treeConst == _inst.size-1);

  _obj = IloExpr(_env);
  for(int i = 0; i < _inst.edges.size(); i++) {
    _obj += _inst.cost[i]*_x[i];
  }
  _model.add(IloMinimize(_env, _obj));

 
  if(_params.model == 'n')
    createPairConflicts();
  else 
    createCliqueConflicts();

  _cplex = IloCplex(_env);
  _cplex.extract(_model);
  
};

void BNCSolver::createPairConflicts() {
  for(auto c : _inst.conflicts) {
    _model.add(_x[c.first]+_x[c.second] <= 1);
  }
  std::cerr << "pair" << std::endl;
}

void BNCSolver::createCliqueConflicts() {
  Instance Ghat;

  Ghat.size = _inst.edges.size();
  Ghat.adjList.resize(Ghat.size);
  Ghat.edges.reserve(_inst.conflicts.size());
  
  for (std::vector<std::pair<int, int>>::iterator it = _inst.conflicts.begin(); 
          it != _inst.conflicts.end(); it++) {
    int u = it->first, v = it->second;

    int id = Ghat.edges.size();
    Edge e (u, v);

    Ghat.edges.push_back(e);
    Ghat.edge2id[e] = id;
    Ghat.adjList[e.u].push_back(id);
    Ghat.adjList[e.v].push_back(id);
  }

  MCSolver mcSolver;

  std::set<std::set<unsigned int>> maximalCliques = mcSolver.solve(&Ghat);

  for (std::set<std::set<unsigned int>>::iterator it = maximalCliques.begin(); 
          it != maximalCliques.end(); it++) {
    std::set<unsigned int> maximalClique = (*it);

    IloExpr lhs (_env);
    double rhs = 1.0;

    for (std::set<unsigned int>::iterator it2 = maximalClique.begin(); it2 != maximalClique.end();
            it2++) {
      unsigned int i = (*it2);

      lhs += _x[i];
    }

    _model.add(lhs <= rhs);
  }

  std::cerr << "clique" << std::endl;
}


void BNCSolver::fixOne(int edge) {
  if(_fixedOnes.count(edge) > 0)
    return;

  _fixedOnes.insert(edge);
  //TODO
  _x[edge].setLB(1);
  std::cerr << "Fixed One " << (int)edge << std::endl;
}

void BNCSolver::fixZero(int edge) {
  if(_fixedZeros.count(edge) > 0)
    return;

  _fixedZeros.insert(edge);
  //TODO
  _x[edge].setUB(0);
  std::cerr << "Fixed Zero " << (int)edge  << std::endl;

}

void BNCSolver::runPreprocessors() {
  bool change = true;
  while(change) {
    change = false;
    for(auto preprocessor : _preprocessors) {
      preprocessor->fixVariables(_inst, _fixedOnes, _fixedZeros);
      for(auto one: preprocessor->ones) {
        change = true;
        fixOne(one);
      }
      for(auto zero: preprocessor->zeros) {
        change = true;
        fixZero(zero);
      }
    }
  }
}

void BNCSolver::config() {
  /* limita o tempo de execucao */
  _cplex.setParam(IloCplex::Param::TimeLimit, _params.timeLimit);

  /* Desabilita o PRESOLVE: o problema da mochila é muito fácil para o CPLEX */
  _cplex.setParam(IloCplex::Param::Preprocessing::Presolve, CPX_OFF);

  /* Desabilita heurísticas: o problema da mochila é muito fácil para o CPLEX */
  _cplex.setParam(IloCplex::Param::MIP::Strategy::HeuristicFreq, -1);
  _cplex.setParam(IloCplex::Param::MIP::Strategy::RINSHeur, -1);
  _cplex.setParam(IloCplex::Param::MIP::Strategy::FPHeur, -1);

  /* Assure linear mappings between the presolved and original models */
  _cplex.setParam(IloCplex::Param::Preprocessing::Linear, 0);

  /* Turn on traditional search for use with control callbacks */
  _cplex.setParam(IloCplex::Param::MIP::Strategy::Search, CPX_MIPSEARCH_TRADITIONAL);

  //disable cplex curs
  _cplex.setParam(IloCplex::Param::MIP::Limits::CutsFactor, 1.0);


  //add preprocessors
  _preprocessors.push_back(new LightEdgePreprocessor());
  _preprocessors.push_back(new BridgePreprocessor());
}

void BNCSolver::createCuts() {
  _cplex.use(UserCuts(_env, _x, _inst));
  _cplex.use(LazyCuts(_env, _x, _inst));
  std::cerr << "cuts enabled" << std::endl;
}

void BNCSolver::createPrimalHeuristic() {
    _cplex.use(PrimalHeuristic(_env, _x, _inst, _fixedZeros, _fixedOnes));
    std::cerr << "primal heuristic enabled" << std::endl;
}

void BNCSolver::processSolution() {
  if(_cplex.solve()) {
    IloNumArray xstar(_env);
    _cplex.getValues(xstar, _x);
    _primalViable = true;
    _solution.fromIloNumArray(xstar, _inst);
    gReport.bestPrimalBound = _solution.value;
  }
  else {
    _primalViable = false;
    _solution.invalid = true;
    gReport.bestPrimalBound = -INF;
  }
}

void BNCSolver::updateReport() {
  gReport.numNodes = _cplex.getNnodes();
  gReport.bestPrimalNode = _cplex.getIncumbentNode();

  gReport.bestDualBound = _cplex.getBestObjValue();

  gReport.totalTime =
    std::chrono::duration_cast<std::chrono::duration<double>>(endTime-startTime).count();
}
void BNCSolver::solve(const Instance & inst, const Params & params) {

  _inst = inst;
  _params = params;
  startTime = std::chrono::high_resolution_clock::now();
  createBaseModel();
  if(params.typeExec == 'c')
    createCuts();

  if(params.heurPrimal)
    createPrimalHeuristic();
  
  config();
  runPreprocessors();
  _cplex.exportModel("first.lp"); 
  processSolution();
  _cplex.exportModel("last.lp"); 
  endTime = std::chrono::high_resolution_clock::now();
  updateReport();
}

bool BNCSolver::isPrimalViable() const {
  return _primalViable;
}

Solution BNCSolver::getSolution() const{
  return _solution;

}

