#include "solver.hpp"
#include "stcut.hpp"
#include "occut.hpp"
#include "primalheur.hpp"

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
  //TODO
  std::cerr << "clique" << std::endl;
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
}

void BNCSolver::createCuts() {
  _cplex.use(STCut(_env, _x, _inst));
  _cplex.use(OCCut(_env, _x, _inst));
  std::cerr << "cuts enabled" << std::endl;
}

void BNCSolver::createPrimalHeuristic() {
    _cplex.use(PrimalHeuristic(_env, _x, _inst));
    std::cerr << "primal heuristic enabled" << std::endl;
}

void BNCSolver::solve(const Instance & inst, const Params & params) {
  _inst = inst;
  _params = params;

  createBaseModel();
  if(params.typeExec == 'c')
    createCuts();

  if(params.heurPrimal)
    createPrimalHeuristic();

  config();
  _cplex.exportModel("lp.lp");

  if(_cplex.solve()) {
    std::cout << "Solução: " << _cplex.getObjValue() << std::endl;
  }
}

