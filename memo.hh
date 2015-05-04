#ifndef __MEMO__HH
#define __MEMO__HH

#include <cstdlib>
#include <iostream>
#include <queue>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/breadth_first_search.hpp>

#include "qset.hh"

/**
Abstract class for memoizing the result of recursive calls,
finding cases that can be efficiently solved,
and optimizations for ordering and pruning our search
 */
class Memoizer
{
   public:
      // pure virtual function
    virtual int subTW(std::set<Vertex> S, Graph G) = 0;
    virtual std::vector<Vertex> orderVertices(std::set<Vertex> S, Graph G) = 0;
    
};





int memoTW(Memoizer* memo, std::set<Vertex> S, Graph G);

int sizeQ(std::set<Vertex> S, Vertex v, Graph G);
int sizeQ(std::set<Vertex> S, Vertex v, Graph G, std::queue<Vertex> open, std::set<Vertex> closed);




#endif

