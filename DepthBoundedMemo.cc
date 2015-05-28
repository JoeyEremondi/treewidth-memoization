#include <vector>
#include <cstdlib>

#include "DepthBoundedMemo.hh"


bool DepthBoundedMemo::shouldCache(std::set<Vertex> S)
{
    return S.size() >= bound;
}


DepthBoundedMemo::DepthBoundedMemo(int boundParam, Graph G) : BasicMemo(G)
{
    bound = boundParam;
    
}

DepthBoundedMemo::~DepthBoundedMemo()
{
}
