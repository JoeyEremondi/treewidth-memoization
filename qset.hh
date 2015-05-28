#ifndef __QSET__HH
#define __QSET__HH

#include <cstdlib>
#include <iostream>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include <unordered_set>


//Adapted from http://stackoverflow.com/questions/14470566/how-to-traverse-graph-in-boost-use-bfs
typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS > Graph;
typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex; 

typedef std::unordered_set<Vertex> VSet;

int sizeQ(VSet &S, Vertex v, Graph &G);

std::string showSet(VSet S);

bool setContains (VSet &S, Vertex v);


#endif
