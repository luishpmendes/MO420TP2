/* implements shortest paths solvers */

#pragma once

#include <queue>
#include <utility>
#include <vector>

#include "constants.hpp"
#include "instance.hpp"

class SPSolver {
    public :
        virtual void solve (Instance * inst, const unsigned int & s, std::vector<double> & d, 
                std::vector<int> & pi) {
        }
};

class DijkstraSPSolver : public SPSolver {
    public :
        void solve (Instance * inst, const unsigned int & s, std::vector<double> & d, 
                std::vector<int> & pi) {
            d = std::vector<double> (inst->size, INF);
            pi = std::vector<int> (inst->size, NIL);
            std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, 
                std::greater<std::pair<int, int>>> Q;

            d[s] = 0;
            Q.push(std::make_pair(0, s));

            while (!Q.empty()) {
                int u = Q.top().second;
                Q.pop();

                for (std::vector<int>::iterator it = inst->adjList[u].begin(); 
                        it != inst->adjList[u].end(); it++) {
                    int e = (*it);
                    int v = inst->getOtherVertice(u, e);
                    double w = inst->cost[inst->edge2id[Edge(u, v)]];
                    
                    if (d[v] > d[u] + w) {
                        d[v] = d[u] + w;
                        pi[v] = u;
                        Q.push(std::make_pair(d[v], v));
                    }
                }
            }
        }
};

