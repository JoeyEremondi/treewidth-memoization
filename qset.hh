#ifndef __QSET__HH
#define __QSET__HH

#include <cstdlib>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include "VSet.hh"
#include "graphTypes.hh"

extern int numQCalled;


//Adapted from http://stackoverflow.com/questions/14470566/how-to-traverse-graph-in-boost-use-bfs


int sizeQ(int n, const VSet &S, Vertex v, const Graph &G);

std::string showSet(VSet S);


#endif
