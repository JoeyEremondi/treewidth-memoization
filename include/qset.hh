#ifndef __QSET__HH
#define __QSET__HH

#include <cstdlib>
#include <iostream>
#include <vector>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include "VSet.hh"
#include "graphTypes.hh"


//Calcualate |Q(S, v)| in G
int sizeQ(int n, const VSet &S, Vertex v, const Graph &G);

//Calcualate |Q(S, v)| in G for each v in G, store in the given vector
void findQvalues(int n, const VSet &S, const Graph &G, std::vector<int>& outValues);


#endif
