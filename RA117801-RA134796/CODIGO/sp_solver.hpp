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
        void solve (Instance * inst, const unsigned int & source, 
                const unsigned int & destination, std::vector<double> & distance, 
                std::vector<int> & predecessor) {
            distance = std::vector<double> (inst->size, INF);
            predecessor = std::vector<int> (inst->size, NIL);
            std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, 
                std::greater<std::pair<double, int>>> Q;

            distance[source] = 0;
            Q.push(std::make_pair(0.0, source));

            while (!Q.empty()) {
                unsigned int u = Q.top().second;
                Q.pop();

                if (u == destination) {
                    break;
                }

                for (std::vector<int>::iterator it = inst->adjList[u].begin(); 
                        it != inst->adjList[u].end(); it++) {
                    int e = (*it);
                    int v = inst->getOtherVertice(u, e);
                    double w = inst->cost[inst->edge2id[Edge(u, v)]];
 
                    if (distance[v] > distance[u] + w + EPS) {
                        distance[v] = distance[u] + w;
                        predecessor[v] = u;
                        Q.push(std::make_pair(distance[v], v));
                    }
                }
            }
        }
};

