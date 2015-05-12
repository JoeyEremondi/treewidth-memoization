#ifndef __DEPTH_BOUND_MEMO__H_
#define __DEPTH_BOUND_MEMO__H_

#include "memo.hh"
#include "qset.hh"
#include "BasicMemo.hh"
#include "NaiveMemo.hh"

#include <map>

/**
A basic memoized treewidth search algorithm, but uses the naive (no memoization)
version when the sets are smaller than the parameter given.
*/
class DepthBoundedMemo : public BasicMemo, public NaiveMemo
{
private:
    unsigned long bruteForceSize;
    
    
public:
    DepthBoundedMemo(unsigned long bound);
    ~DepthBoundedMemo();
    int subTW(std::set<Vertex> S, Graph G);
    //std::vector<Vertex> orderVertices(std::set<Vertex> S, Graph G);
    
};

#endif   
