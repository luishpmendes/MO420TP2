#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>

#include "ilcplex/ilocplex.h"
#include "instance.hpp"
#include "report.hpp"
#include "solver.hpp"

#include "global.hpp"

Report gReport;
Info gInfo;

int main (int argc, char * argv[]) {
    Params params;


    /* tipoExec é o tipo de execução dado por um caracter tal que: “e” equivale a um */
    /* algoritmo branch-and-bound puro e “c” equivale a um algoritmo branch-and-cut */
    if(argc > 1)
      params.typeExec = argv[1][0];

    /* model é o tipo do modelo utilizado, sendo “n” o modelo (5) e “c” o modelo (6) */
    if(argc > 2)
      params.model = argv[2][0];

    /* timeLimit é o tempo máximo de execucão do algoritmo em segundos dado por um número */
    /* inteiro positivo */
    if(argc > 3)
      params.timeLimit = atoi(argv[3]);

    /* heurPrimal é um valor binário que vale “1” se a heurística primal for usada e */
    /* “0” caso contrário */
    if(argc > 4)
      params.heurPrimal = atoi(argv[4]);

    /* inputFilePath é o nome do arquivo de entrada */
    if(argc > 5)
      params.filename = std::string(argv[5]);
    else
      params.filename = "../../instances/exemplo.gcc"; 

    Instance inst;
    inst.load(params.filename.c_str());

    BNCSolver solver;
    solver.solve(inst, params);
    solver.getSolution().write(inst, params.filename+".sol");
    gReport.write(params, params.filename+".est");
    //gInfo.write(filename+".info");
    return 0;
}

