#pragma once

#include <algorithm>
#include <set>
#include <vector>

#include "instance.hpp"
#include "mergefind.hpp"

#include "ilcplex/ilocplex.h"

struct RegAux {
    double valor;
    int indice;
    friend bool operator < (const RegAux & a, const RegAux & b) {
        return (a.valor > b.valor);
    }
};

bool fixSolution(const Instance & inst, const std::vector<double> & x, 
        const std::set<int> & fixedZeros, const std::set<int> & fixedOnes, 
        std::set<int> & primalSolution) {
    /* vetor de registros */
    std::vector<RegAux> xh(inst.edges.size());

    for (unsigned int i = 0; i < inst.edges.size(); i++) {
        xh[i].valor = x[i];
        xh[i].indice = i;
    }

    /* ordena em ordem decrescente de valores */
    sort(xh.begin(), xh.end());

    MergeFind mergeFind (inst.size);

    /* Adiciona as arestas fixadas na solução primal */
    for (std::set<int>::iterator it = fixedOnes.begin(); it != fixedOnes.end(); it++) {
        int j = (*it);

        Edge e = inst.edges[j];

        int u = e.u, v = e.v;

        if (mergeFind.merge(u, v)) {
            primalSolution.insert(j);
        }
    }

    /* adiciona as arestas e com maior valor x[e] na solução primal */
    /* sem criar ciclos nem conflitos */
    for (unsigned int i = 0; i < xh.size() && (int) primalSolution.size() < inst.size - 1; i++) {
        int j = xh[i].indice;

        Edge e = inst.edges[j];

        if (fixedZeros.find(j) == fixedZeros.end()) {
            int u = e.u, v = e.v;

            if (mergeFind.find(u) != mergeFind.find(v)) {
                bool conflicts = false;

                for (std::set<int>::iterator it2 = primalSolution.begin(); 
                        it2 != primalSolution.end() && !conflicts; it2++) {
                    int k = (*it2);

                    if (inst.conflictsSet.find(std::pair<int, int>(j, k)) != 
                            inst.conflictsSet.end() || 
                            inst.conflictsSet.find(std::pair<int, int>(k, j)) != 
                            inst.conflictsSet.end()) {
                        conflicts = true;
                    }
                }

                if (!conflicts) {
                    mergeFind.merge(u, v);
                    primalSolution.insert(j);
                }
            }
        }
    }

    if ((int) primalSolution.size() == (inst.size - 1)) {
        return true;
    }

    /* adiciona as arestas e com maior valor x[e] na solução primal */
    /* sem criar ciclos, mas podendo criar conflitos */
    for (unsigned int i = 0; i < xh.size() && (int) primalSolution.size() < inst.size - 1; i++) {
        int j = xh[i].indice;

        Edge e = inst.edges[j];

        if (fixedZeros.find(j) == fixedZeros.end()) {
            int u = e.u, v = e.v;

            if (mergeFind.merge(u, v)) {
                primalSolution.insert(j);
            }
        }
    }

    /* remove da solução primal as arestas não fixadas */
    /* que estão envolvidas em algum conflito */
    for (std::set<std::pair<int, int>>::iterator it = inst.conflictsSet.begin(); 
            it != inst.conflictsSet.end(); it++) {
        int i = it->first, j = it->second;

        if (primalSolution.find(i) != primalSolution.end() && 
                primalSolution.find(j) != primalSolution.end()) {
            if (fixedOnes.find(i) == fixedOnes.end()) {
                primalSolution.erase(i);
            }

            if (fixedOnes.find(j) == fixedOnes.end()) {
                primalSolution.erase(j);
            }
        }
    }

    mergeFind = MergeFind(inst.size);

    for (std::set<int>::iterator it = primalSolution.begin(); it != primalSolution.end(); it++) {
        int j = (*it);

        Edge e = inst.edges[j];

        int u = e.u, v = e.v;

        mergeFind.merge(u, v);
    }

    std::vector<std::pair<double, int>> wid;

    for (int i = 0; i < (int) inst.edges.size(); i++) {
        if (fixedZeros.find(i) == fixedZeros.end() && 
                primalSolution.find(i) == primalSolution.end()) {
            bool isConflicting = false;

            for (std::set<int>::iterator it = primalSolution.begin(); 
                    it != primalSolution.end() && !isConflicting; it++) {
                int j = (*it);

                if (inst.conflictsSet.find(std::pair<int, int>(i, j)) != inst.conflictsSet.end() 
                        || inst.conflictsSet.find(std::pair<int, int>(j, i)) != 
                        inst.conflictsSet.end()) {
                    isConflicting = true;
                }
            }

            if (!isConflicting) {
                wid.push_back(std::pair<double, int>(inst.cost[i], i));
            }
        }
    }

    std::sort(wid.begin(), wid.end());

    for (std::vector<std::pair<double, int>>::iterator it = wid.begin(); it != wid.end(); it++) {
        int i = it->second;

        Edge e = inst.edges[i];

        int u = e.u, v = e.v;

        bool conflicts = false;

        for (std::set<int>::iterator it2 = primalSolution.begin(); 
                it2 != primalSolution.end() && !conflicts; it2++) {
            int j = (*it2);

            if (inst.conflictsSet.find(std::pair<int, int>(i, j)) != inst.conflictsSet.end() || 
                        inst.conflictsSet.find(std::pair<int, int>(j, i)) != 
                        inst.conflictsSet.end()) {
                conflicts = true;
            }
        }

        if (!conflicts && mergeFind.merge(u, v)) {
            primalSolution.insert(i);
        }
    }

    if ((int) primalSolution.size() == (inst.size - 1)) {
        return true;
    }

    return false;
}

ILOHEURISTICCALLBACK4(PrimalHeuristic, IloBoolVarArray, x, const Instance &, inst, const std::set<int> &, fixedZeros, const std::set<int> &, fixedOnes) {
    IloEnv env = getEnv();

    /* recupera solucação da relaxação */
    IloNumArray val(env);
    getValues(val, x);

    std::vector<double> xVecDouble (inst.edges.size());

    for (unsigned int j = 0; j < inst.edges.size(); j++) {
        xVecDouble[j] = ((double) val[j]);
    }

    std::set<int> primalSolution;
    if (fixSolution(inst, xVecDouble, fixedZeros, fixedOnes, primalSolution)) {
        for (int j = 0; j < (int) inst.edges.size(); j++) {
            if (primalSolution.find(j) != primalSolution.end()) {
                val[j] = 1.0;
            } else {
                val[j] = 0.0;
            }
        }
    }

    std::cerr << "primalHeuristic" << std::endl;
}

