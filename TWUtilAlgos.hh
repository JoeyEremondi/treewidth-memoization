#ifndef __TW__UTIL__ALGOS__H
#define __TW__UTIL__ALGOS__H

#include "qset.hh"
#include <vector>

//Find the width of a given permutation
//Useful in finding heuristic upper bounds for treewidth
//TODO check if need copy?
int permutTW(std::vector<Vertex> Svec, Graph G);

//Return the edge-complement of a graph
void complement_graph(const Graph& g, Graph& gp);


//We calculate maxClique by calculating indSet on the complement
VSet exactMaxIndSet(Graph G);


VSet maxIndSetHelper(VSet S, Graph G);

//Exact algorithm for max clique
//We will see if this is fast enough for lower bounds
VSet exactMaxClique(Graph G);

//min-degree heuristic for finding a clique

VSet approxIndSetHelper(VSet S, Graph G);

VSet approxMaxIndSet(Graph G);

VSet approxMaxClique(Graph G);

//Use known techniques to find the lower-bound for the treewidth of a subgraph of a graph
int subgraphTWLowerBound(VSet S, Graph G);





#endif
