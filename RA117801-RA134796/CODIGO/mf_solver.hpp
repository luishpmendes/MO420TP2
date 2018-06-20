#include "instance.hpp"
#include "constants.hpp"
#include <list>
#include <vector>
#include "compare_double.hpp"


//Modificação docódigo disponível na página GeeksForGeeks:
//https://www.geeksforgeeks.org/dinics-algorithm-maximum-flow/

// A structure to represent a edge between
// two vertex
struct FlowEdge
{
    int v ;  // Vertex v (or "to" vertex)
             // of a directed edge u-v. "From"
             // vertex u can be obtained using
             // index in adjacent array.
 
    double flow ; // flow of data in edge
 
    double C;    // capacity
 
    int rev ; // To store index of reverse
              // edge in adjacency list so that
              // we can quickly find it.
};
 
// Residual FlowGraph
class FlowGraph
{
    int _n; // number of vertex
    std::vector<int> _level ; // stores level of a node
    std::vector<std::vector<FlowEdge >> _adj;
public:
    FlowGraph(int n)
    {
        _n = n;
        _adj = std::vector<std::vector<FlowEdge>>(n);
        _level = std::vector<int>(n);
    }
 
    // add edge to the graph
    void addFlowEdge(int u, int v, double C)
    {
        // Forward edge : 0 flow and C capacity
        FlowEdge a{v, 0, C, (int)_adj[v].size()};
 
        // Back edge : 0 flow and 0 capacity
        FlowEdge b{u, 0, 0, (int)_adj[u].size()};
 
        _adj[u].push_back(a);
        _adj[v].push_back(b); // reverse edge
    }
 
    bool BFS(int s, int t);
    double sendFlow(int s, double flow, int t, std::vector<int> &start);
    double DinicMaxflow(int s, int t);
    
    std::vector<int> getCutSide(int s);
    int CutDFS(int u, std::vector<bool> & vis);
};
 
// Finds if more flow can be sent from s to t.
// Also assigns levels to nodes.
bool FlowGraph::BFS(int s, int t)
{
    for (int i = 0 ; i < _n ; i++)
        _level[i] = -1;
 
    _level[s] = 0;  // Level of source vertex
 
    // Create a queue, enqueue source vertex
    // and mark source vertex as visited here
    // level[] array works as visited array also.
    std::list< int > q;
    q.push_back(s);
 
    std::vector<FlowEdge>::iterator i ;
    while (!q.empty())
    {
        int u = q.front();
        q.pop_front();
        for (FlowEdge &e : _adj[u])
        {
            if (_level[e.v] < 0  && e.flow < e.C-EPS)
            {
                // Level of current vertex is,
                // level of parent + 1
                _level[e.v] = _level[u] + 1;
 
                q.push_back(e.v);
            }
        }
    }
 
    // IF we can not reach to the sink we
    // return false else true
    return _level[t] < 0 ? false : true ;
}
 
// A DFS based function to send flow after BFS has
// figured out that there is a possible flow and
// constructed levels. This function called multiple
// times for a single call of BFS.
// flow : Current flow send by parent function call
// start[] : To keep track of next edge to be explored.
//           start[i] stores  count of edges explored
//           from i.
//  u : Current vertex
//  t : Sink
double FlowGraph::sendFlow(int u, double flow, int t, std::vector<int> &start)
{
    // Sink reached
    if (u == t)
        return flow;
    // Traverse all adjacent edges one -by - one.
    for (  ; start[u] < _adj[u].size(); start[u]++)
    {
        // Pick next edge from adjacency list of u
        FlowEdge &e = _adj[u][start[u]]; 
                                     
        if (_level[e.v] == _level[u]+1 && e.flow < e.C-EPS)
        {
            // find minimum flow from u to t
            double curr_flow = std::min(flow, e.C - e.flow);
 
            double temp_flow = sendFlow(e.v, curr_flow, t, start);
 
            // flow is greater than zero
            if (temp_flow > EPS)
            {
                // add flow  to current edge
                e.flow += temp_flow;
 
                // subtract flow from reverse edge
                // of current edge
                _adj[e.v][e.rev].flow -= temp_flow;
                return temp_flow;
            }
        }
    }
 
    return 0;
}
 
// Returns maximum flow in graph
double FlowGraph::DinicMaxflow(int s, int t)
{
    // Corner case
    if (s == t)
        return -1;
 
    double total = 0;  // Initialize result
 
    // Augment the flow while there is path
    // from source to sink
    while (BFS(s, t) == true)
    {
        // store how many edges are visited
        // from V { 0 to V }
        std::vector<int> start(_n+1);
 
        // while flow is not zero in graph from S to D
        while (true) {
            // Add path flow to overall flow
            double flow = sendFlow(s, INF, t, start);
            if(equalDouble(flow, 0))
              break;
            total += flow;
        }
    }
 
    // return maximum flow
    return total;
}

// returns one of the subsets of vertices separated by the cut
std::vector<int> FlowGraph::getCutSide(int s) {
  std::vector<int> subtour;

  std::vector<bool> vis(_n, false);
  int count = CutDFS(s, vis);
  bool mask = true;
  if(count < 3)
    mask = false;
  
  for(int u = 0;  u < _n; u++) {
    if(vis[u] == mask)
      subtour.push_back(u);
  }
  return subtour;
}

// visit all nodes reachable from u without crossing a cut edge
int FlowGraph::CutDFS(int u, std::vector<bool> & vis) {
  if(vis[u] == true)
    return 0;
  int count = 1;
  vis[u] = true;
  for(const FlowEdge & e : _adj[u]) {
    if(e.C > EPS && e.flow+EPS < e.C)
      count += CutDFS(e.v, vis);
  }
  return count;
}
