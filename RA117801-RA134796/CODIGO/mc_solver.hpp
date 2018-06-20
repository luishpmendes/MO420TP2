#include <vector>
#include <set>

#include "instance.hpp"

class MCSolver {
    public :
        std::set<std::set<unsigned int>> solve (Instance * inst) {
            std::set<std::set<unsigned int>> maximalCliques;

            std::vector<bool> isInAnyClique (inst->edges.size(), false);

            /* for each vertex u and for each v ∈ N[u] */
            /* that is not in any clique generated so far */
            for (unsigned int e = 0; e < inst->edges.size(); e++) {
                if (!isInAnyClique[e]) {
                    unsigned int u = inst->edges[e].u, v = inst->edges[e].v;

                    /* form the partial clique {u, v} */
                    std::set<unsigned int> maximalClique;

                    maximalClique.insert(u);
                    maximalClique.insert(v);

                    isInAnyClique[e] = true;

                    bool isExpanded = true;

                    /* which is then expanded until it is maximal */
                    while (isExpanded) {
                        isExpanded = false;

                        for (std::set<unsigned int>::iterator it = maximalClique.begin(); 
                                it != maximalClique.end(); it++ ) {
                            unsigned int w = (*it);

                            for (std::vector<int>::iterator it2 = inst->adjList[w].begin(); 
                                    it2 != inst->adjList[w].end(); it2++) {
                                int x = inst->getOtherVertice(w, (*it2));

                                bool canBeExpanded = maximalClique.find(x) == maximalClique.end();

                                for (std::set<unsigned int>::iterator it3 = maximalClique.begin();
                                        it3 != maximalClique.end() && canBeExpanded; it3++) {
                                    unsigned int y = (*it3);

                                    if (inst->edge2id.find(Edge (x, y)) == inst->edge2id.end()) {
                                        canBeExpanded = false;
                                    }
                                }

                                if (canBeExpanded) {
                                    for (std::set<unsigned int>::iterator it3 = 
                                            maximalClique.begin(); it3 != maximalClique.end(); 
                                            it3++) {
                                        unsigned int y = (*it3);

                                        isInAnyClique[inst->edge2id[Edge (x, y)]] = true;
                                    }

                                    maximalClique.insert(x);

                                    isExpanded = true;
                                }
                            }
                        }
                    }
                    maximalCliques.insert(maximalClique);
                }
            }

            return maximalCliques;
        }
};

