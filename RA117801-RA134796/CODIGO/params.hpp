#pragma once
#include <string>

class Params {
  public:
    /* tipoExec é o tipo de execução dado por um caracter tal que: “e” equivale a um */
    /* algoritmo branch-and-bound puro e “c” equivale a um algoritmo branch-and-cut */
  char typeExec;
    /* model é o tipo do modelo utilizado, sendo “n” o modelo (5) e “c” o modelo (6) */
  char model;
    /* timeLimit é o tempo máximo de execucão do algoritmo em segundos dado por um número */
    /* inteiro positivo */
  int timeLimit;
    /* heurPrimal é um valor binário que vale “1” se a heurística primal for usada e */
    /* “0” caso contrário */
  bool heurPrimal;
  std::string filename;

  Params() {
    typeExec = 'e';
    model = 'c';
    timeLimit = 60;
    heurPrimal = false;
  }
  std::string toString() {
    std::string r;

    return r;
  }

};
