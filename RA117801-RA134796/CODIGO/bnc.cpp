#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <set>

#include "ilcplex/ilocplex.h"

using namespace std;

typedef struct {
    unsigned int u; /* edge origin */
    unsigned int v; /* edge destination */
    double w; /* edge weight */
} Edge;

typedef struct {
    Edge e;
    Edge f;
} ConflictingPair;

bool edgeComparator (Edge e, Edge f) {
    if (e.u == f.u) {
        return e.v < f.v;
    }

    return e.u < f.u;
}

bool conflictingPairComparator (ConflictingPair a, ConflictingPair b) {
    if (a.e.u == b.e.u) {
        if (a.e.v == b.e.v) {
            if (a.f.u == b.f.u) {
                return a.f.v < b.f.v;
            }

            return a.f.u < b.f.u;
        }

        return a.e.v < b.e.v;
    }

    return a.e.u < b.e.u;
}

bool readInput (unsigned int * n, set <Edge, bool (*) (Edge, Edge)> * E, set <ConflictingPair, 
        bool (*) (ConflictingPair, ConflictingPair)> * S, char * inputFilePath) {
    ifstream inputFileStream(inputFilePath, ifstream::in);

    if (!inputFileStream.is_open()) {
        return false;
    }

    string line;

    /* Linhas de comentário, iniciadas com o caractere ‘#’. */
    do {
        getline(inputFileStream, line);
    } while (line[0] == '#');

    /* Uma linha com o nome do arquivo. */

    unsigned int m, p;

    /* Uma linha com o número de vértices do grafo (n). */
    inputFileStream >> (*n);

    /* Uma linha com o número de arestas do grafo (m). */
    inputFileStream >> m;

    /* Uma linha com a quantidade de pares de arestas conflitantes (p). */
    inputFileStream >> p;

    bool (*edgeComparatorPointer) (Edge, Edge) = edgeComparator;
    (*E) = set <Edge, bool (*) (Edge, Edge)> (edgeComparatorPointer);

    bool (*conflictingPairComparatorPointer) (ConflictingPair, ConflictingPair) = 
        conflictingPairComparator;
    (*S) = set <ConflictingPair, bool (*) (ConflictingPair, ConflictingPair)> (
            conflictingPairComparatorPointer);

    /* m linhas, cada uma identificando uma aresta e = (u, v), de custo ce, sendo 0 ≤ u, */
    /* v ≤ n − 1 e 0 ≤ ce ≤ 500, no seguinte formato: u v ce */
    for (unsigned int j = 0; j < m; j++) {
        int u, v;
        double w;

        inputFileStream >> u >> v >> w;

        Edge e;

        if (u < v) {
            e.u = u;
            e.v = v;
        } else {
            e.u = v;
            e.v = u;
        }

        e.w = w;

        (*E).insert(e);
    }

    /* p linhas, cada uma identificando um par de arestas conflitantes {e, f}, sendo */
    /* e = (u, v), f = (x, y) e 0 ≤ u, v, x, y ≤ n − 1, no seguinte formato: u v x y */
    for (unsigned int j = 0; j < p; j++) {
        Edge e, f;
        unsigned int u, v;

        inputFileStream >> u >> v;

        if (u < v) {
            e.u = u;
            e.v = v;
        } else {
            e.u = v;
            e.v = u;
        }

        inputFileStream >> u >> v;

        if (u < v) {
            f.u = u;
            f.v = v;
        } else {
            f.u = v;
            f.v = u;
        }

        set <Edge, bool (*) (Edge, Edge)> :: iterator it = (*E).find(e);

        if (it != (*E).end()) {
            e.w = it->w;
        }

        it = (*E).find(f);

        if (it != (*E).end()) {
            f.w = it->w;
        }

        ConflictingPair cp;

        if (edgeComparator(e, f)) {
            cp.e = e;
            cp.f = f;
        } else {
            cp.e = f;
            cp.f = e;
        }

        (*S).insert(cp);
    }

    return true;
}

void bnc (char tipoExec, char model, unsigned int timeLimit, bool heurPrimal, unsigned int n, 
        set <Edge, bool (*) (Edge, Edge)> E, 
        set <ConflictingPair, bool (*) (ConflictingPair, ConflictingPair)> S, 
        set <Edge, bool (*) (Edge, Edge)> * bestPrimalSolution, 
        unsigned int * bestPrimalBoundValue, unsigned int * totalCuts1, 
        unsigned int * totalCuts2, double * firstRelaxationValue, double * rootValue, 
        unsigned int * totalNodes, unsigned int * bestPrimalSolutionNode, 
        double * bestDualBoundValue, chrono::high_resolution_clock::time_point tBegin) {
    /* TODO */
}

bool writeSol (char tipoExec, char model, unsigned int timeLimit, bool heurPrimal, 
        char * inputFilePath, unsigned int n, 
        set <Edge, bool (*) (Edge, Edge)> bestPrimalSolution, unsigned int bestPrimalBoundValue) {
    string solFilePath (inputFilePath);

    solFilePath += ".sol";

    ofstream solFileStream(solFilePath, ofstream::out);

    if (!solFileStream.is_open()) {
        return false;
    }

    /* uma linha contendo os parâmetros de entrada, na mesma ordem em que aparecem na linha de */
    /* comando descrita acima, sendo todos os parâmetros separados por um (caracter) branco */
    solFileStream << tipoExec << ' ' << model << ' ' << timeLimit << ' ' << heurPrimal << ' ' << 
        inputFilePath << endl;

    if (bestPrimalSolution.size() + 1 == n) {
        /* n − 1 linhas contendo todos os pares de vértices i e j, com i < j, tais que a */
        /* aresta (i, j) de G está na árvore geradora de menor custo encontrada pelo */
        /* algoritmo */
        for (set <Edge>::iterator it = bestPrimalSolution.begin(); 
                it != bestPrimalSolution.end(); it++) {
            Edge e = (*it);

            solFileStream << e.u << ' ' << e.v << endl;
        }

        /* uma linha contendo o valor desta solução */
        solFileStream << bestPrimalBoundValue << endl;
    } else {
        /* Se nenhuma solução primal for encontrada, todos os pares de valores inteiros nas */
        /* n − 1 linhas correspondentes às arestas da solução devem ser iguais a “-1 -1” */
        for (unsigned int i = 0; i + 1 < n; i++) {
            solFileStream << -1 << ' ' << -1 << endl;
        }

        /* e a linha de custo da solução deverá conter o valor “-1” */
        solFileStream << -1 << endl;
    }

    return true;
}

bool writeEst (char tipoExec, char model, unsigned int timeLimit, bool heurPrimal, 
        char * inputFilePath, unsigned int totalCuts1, unsigned int totalCuts2, 
        double firstRelaxationValue, double rootValue, unsigned int totalNodes, 
        unsigned int bestPrimalSolutionNode, unsigned int bestPrimalBoundValue, 
        double bestDualBoundValue, chrono::high_resolution_clock::time_point tBegin) {
    string estFilePath (inputFilePath);

    estFilePath += ".est";

    ofstream estFileStream(estFilePath, ofstream::out);

    if (!estFileStream.is_open()) {
        return false;
    }

    chrono::high_resolution_clock::time_point tCurrent = chrono::high_resolution_clock::now();
    chrono::seconds elapsedTime = chrono::duration_cast <chrono::seconds> (tCurrent - tBegin);

    /* uma linha contendo os parâmetros de entrada, na mesma ordem em que aparecem na linha de */
    /* comando descrita acima, sendo todos os parâmetros separados por um (caracter) branco */
    estFileStream << tipoExec << ' ' << model << ' ' << timeLimit << ' ' << heurPrimal << ' ' << 
        inputFilePath << endl;

    /* total de desigualdades (1) que foram separadas durante a execução do algoritmo */
    estFileStream << totalCuts1 << endl;

    /* total de desigualdades oci que foram separadas durante a execução do algoritmo */
    estFileStream << totalCuts2 << endl;

    /* valor da FO da primeira relaxação */
    estFileStream << fixed << setprecision(6) << firstRelaxationValue << endl;

    /* valor da FO no nó raiz */
    estFileStream << fixed << setprecision(6) << rootValue << endl;

    /* total de nós explorados */
    estFileStream << totalNodes << endl;

    /* nó onde encontrou a melhor solução inteira */
    estFileStream << bestPrimalSolutionNode << endl;

    /* valor da melhor solução inteira */
    estFileStream << bestPrimalBoundValue << endl;

    /* melhor limitante dual */
    estFileStream << fixed << setprecision(6) << bestDualBoundValue << endl;

    /* o tempo de execução em segundos */
    estFileStream << fixed << setprecision(6) << ((double) elapsedTime.count()) << endl;

    return true;
}

int main (int argc, char * argv[]) {
    chrono::high_resolution_clock::time_point tBegin = chrono::high_resolution_clock::now();

    if (argc != 6) {
        cerr << "Invalid arguments!" << endl;
        exit(EXIT_FAILURE);
    }

    char tipoExec, model;
    unsigned int timeLimit;
    bool heurPrimal;
    char * inputFilePath;

    /* tipoExec é o tipo de execução dado por um caracter tal que: “e” equivale a um */
    /* algoritmo branch-and-bound puro e “c” equivale a um algoritmo branch-and-cut */
    tipoExec = argv[1][0];

    /* model é o tipo do modelo utilizado, sendo “n” o modelo (5) e “c” o modelo (6) */
    model = argv[2][0];

    /* timeLimit é o tempo máximo de execucão do algoritmo em segundos dado por um número */
    /* inteiro positivo */
    timeLimit = atoi(argv[3]);

    /* heurPrimal é um valor binário que vale “1” se a heurística primal for usada e */
    /* “0” caso contrário */
    heurPrimal = atoi(argv[4]);

    /* inputFilePath é o nome do arquivo de entrada */
    inputFilePath = argv[5];

    unsigned int n;
    set <Edge, bool (*) (Edge, Edge)> E;
    set <ConflictingPair, bool (*) (ConflictingPair, ConflictingPair)> S;

    if (!readInput(&n, &E, &S, inputFilePath)) {
        cerr << "Error while reading input!" << endl;
        exit(EXIT_FAILURE);
    }

    set <Edge, bool (*) (Edge, Edge)> bestPrimalSolution;
    double firstRelaxationValue, rootValue, bestDualBoundValue;
    unsigned int totalCuts1, totalCuts2, totalNodes, bestPrimalSolutionNode, bestPrimalBoundValue;

    bnc(tipoExec, model, timeLimit, heurPrimal, n, E, S, &bestPrimalSolution, 
            &bestPrimalBoundValue, &totalCuts1, &totalCuts2, &firstRelaxationValue, &rootValue, 
            &totalNodes, &bestPrimalSolutionNode, &bestDualBoundValue, tBegin);

    if (!writeSol(tipoExec, model, timeLimit, heurPrimal, inputFilePath, n, bestPrimalSolution, 
                bestPrimalBoundValue)) {
        cerr << "Error while writing sol!" << endl;
        exit(EXIT_FAILURE);
    }

    if (!writeEst(tipoExec, model, timeLimit, heurPrimal, inputFilePath, totalCuts1, totalCuts2, 
                firstRelaxationValue, rootValue, totalNodes, bestPrimalSolutionNode, 
                bestPrimalBoundValue, bestDualBoundValue, tBegin)) {
        cerr << "Error while writing est!" << endl;
        exit(EXIT_FAILURE);
    }

    return 0;
}

