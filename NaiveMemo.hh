#ifndef __NAIVE_MEMO__H_
#define __NAIVE_MEMO__H_
#include "memo.hh"

class NaiveMemo : public Memoizer
{    
public:
      // pure virtual function
    int subTW(std::set<Vertex> S, Graph G);
    std::vector<Vertex> orderVertices(std::set<Vertex> S, Graph G);
    
};

#endif    
