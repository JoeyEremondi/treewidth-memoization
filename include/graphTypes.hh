#ifndef __GRAPH__TYPES__H__
#define __GRAPH__TYPES__H__

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>

//Some nice type abbreviations for the Boost graph types
//We use vector-based graphs, since we don't often need to modify them but want fast lookups

typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS > Graph;
typedef boost::graph_traits<Graph>::edge_descriptor Edge;
typedef boost::graph_traits<Graph>::vertex_descriptor Vertex;

const int NO_WIDTH = -1;

#endif
