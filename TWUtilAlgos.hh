#ifndef __TW__UTIL__ALGOS__H
#define __TW__UTIL__ALGOS__H

#include "qset.hh"
#include <vector>

//Find the width of a given permutation
//Useful in finding heuristic upper bounds for treewidth
//TODO check if need copy?
int permutTW(VSet vsetArg, const std::vector<Vertex>& Svec, const Graph& G);

//Return the edge-complement of a graph
std::pair<Graph, std::map<Vertex, Vertex>> complement_graph(const Graph& G);


//We calculate maxClique by calculating indSet on the complement
VSet exactMaxIndSet(const Graph& G);


VSet maxIndSetHelper(VSet S, const Graph& G);

//Exact algorithm for max clique
//We will see if this is fast enough for lower bounds
VSet exactMaxClique(const Graph& G);

//min-degree heuristic for finding a clique

VSet approxIndSetHelper(VSet S, const Graph& G);

VSet approxMaxIndSet(const Graph& G);

VSet approxMaxClique(const Graph& G);

//Use known techniques to find the lower-bound for the treewidth of a subgraph of a graph
int subgraphTWLowerBound(VSet S, const Graph& G, const Graph& GC);

std::pair<Vertex, int> minDegreeSubgraphVert(VSet S, const Graph& G);

//Used for Delta2 degeneracy, return the min-degree vertex of a subgraph
//that isn't the given vertex v
std::pair<Vertex, int> minDegreeVertExcept(Vertex v, VSet S, const Graph& G);

int d2degen(VSet Sarg, const Graph& G);

int MMD(VSet Sarg, const Graph& G);


int subgraphDegree(Vertex v, VSet S, const Graph& G);




#endif
