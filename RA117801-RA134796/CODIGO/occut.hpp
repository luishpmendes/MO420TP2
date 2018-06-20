#include <algorithm>
#include <set>

#include "ilcplex/ilocplex.h"

#include "instance.hpp"
#include "sp_solver.hpp"

void occuts (const IloNumArray & val, const Instance & inst, std::set<std::set<unsigned int>> & oddCycles) {
    oddCycles = std::set<std::set<unsigned int>> ();

    /* We start by defining a new weight function w for adjacencies in the conflict graph Ĝ(E,C) */
    /* let w(u, v) = (1−xu−xv )/2 for each {u, v} ∈ C */
    Instance Ghat;

    Ghat.size = inst.edges.size();
    Ghat.adjList.resize(Ghat.size);
    Ghat.edges.reserve(inst.conflicts.size());
    Ghat.cost.reserve(inst.conflicts.size());

    for (std::vector<std::pair<int, int>>::const_iterator it = inst.conflicts.begin(); 
            it != inst.conflicts.end(); it++) {
        int u = it->first, v = it->second;
        double w = (1.0 - val[u] - val[v]) / 2.0;

        int id = Ghat.edges.size();
        Edge e (u, v);

        Ghat.edges.push_back(e);
        Ghat.cost.push_back(w);
        Ghat.edge2id[e] = id;
        Ghat.adjList[e.u].push_back(id);
        Ghat.adjList[e.v].push_back(id);
    }

    /* We also construct an auxiliary bipartite graph H by duplicating Ĝ as follows: */
    /* H has two vertices u+ and u− for each u ∈ E, */
    /* as well as edges {u+, v−} and {u−, v+} for each {u, v} ∈ C, */
    /* both with weight w(u, v). */
    Instance H;

    H.size = 2 * Ghat.size;
    H.adjList.resize(H.size);
    H.edges.reserve(2 * Ghat.edges.size());
    H.cost.reserve(2 * Ghat.cost.size());

    for (std::vector<Edge>::iterator it = Ghat.edges.begin(); it != Ghat.edges.end(); it++) {
        unsigned int u = it->u, v = it->v;

        unsigned int uMinus = u, uPlus = u + inst.edges.size();
        unsigned int vMinus = v, vPlus = v + inst.edges.size();

        double w = Ghat.cost[Ghat.edge2id[(*it)]];

        unsigned int id = H.edges.size();
        Edge e (uPlus, vMinus);

        H.edges.push_back(e);
        H.cost.push_back(w);
        H.edge2id[e] = id;
        H.adjList[e.u].push_back(id);
        H.adjList[e.v].push_back(id);

        id = H.edges.size();
        Edge f (uMinus, vPlus);

        H.edges.push_back(f);
        H.cost.push_back(w);
        H.edge2id[f] = id;
        H.adjList[f.u].push_back(id);
        H.adjList[f.v].push_back(id);
    }

    std::vector<double> d;
    std::vector<int> pi;

    DijkstraSPSolver spSolver;

    for (unsigned int u = 0; u < Ghat.size; u++) {
        unsigned int uMinus = u, uPlus = u + inst.edges.size();

        /* Then, for each u ∈ E, we compute a shortest(u+, u−) path in the auxiliary graph H. */
        spSolver.solve(&H, uMinus, uPlus, d, pi);

        if (pi[uPlus] >= 0) {
            std::vector<unsigned int> walk;

            /* By omitting the + and − indices, the path corresponds to a closed odd-walk in Ĝ. */
            while (uMinus != uPlus) {
                uPlus = pi[uPlus];
                walk.push_back(uPlus % inst.edges.size());
            }

            std::reverse(walk.begin(), walk.end());

            /* An odd-cycle is possibly retrieved after removing such repetitions, */
            /* by inspecting the vertices in the sequence */
            for (unsigned int i = 0; i < walk.size(); i++) {
                unsigned int j;

                for (j = walk.size() - 1; j > i; j--) {
                    if (walk[i] == walk[j]) {
                        break;
                    }
                }

                if (i < j) {
                    walk.erase(walk.begin() + i, walk.begin() + j);
                }
            }

            bool isOddCycle = ((walk.size() & 1) == 1);
            double wU = 0.0;

            std::set<unsigned int> oddCycle;

            /* The weight of any such odd-cycle U ⊂ C in the conflict graph is w(U) = {i,j} ∈ U w(i, j) */
            for (unsigned int i = 0; i < walk.size() && isOddCycle; i++) {
                unsigned int j = (i + 1) % walk.size();

                oddCycle.insert(walk[i]);

                Edge e (walk[i], walk[j]);

                if (Ghat.edge2id.find(e) != Ghat.edge2id.end()) {
                    wU += Ghat.cost[Ghat.edge2id[e]];
                } else {
                    isOddCycle = false;
                }
            }

            /* x violates the corresponding odd-cycle inequality if and only if w(U) < 1/2 */
            if (isOddCycle && wU < 0.5) {
                oddCycles.insert(oddCycle);
            }
        }
    }
}

