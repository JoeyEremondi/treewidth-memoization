#include <vector>
#include <cstdlib>

#include "BasicMemo.hh"

//Ordered vertices: basic version doesn't order anything
std::vector<Vertex> BasicMemo::orderVertices(VSet S)
{
    std::vector<Vertex> v(S.begin(), S.end());
    return v;
}

//Basic version: upper bound is the size of the set
int BasicMemo::upperBound(VSet S)
{
    return numVerts;
}

//Basic version: lower bound is always 1, nothing fancy
int BasicMemo::lowerBound(VSet S)
{
    return 1;
}

//Always cache for basic memoization
bool BasicMemo::shouldCache(VSet S)
{
    return true;
}

//Never clean in the basic version
bool BasicMemo::needsCleaning()
{
    return false;
}

//Should never get called
std::vector<VSet> BasicMemo::setsToForget(VSet S)
{
    return std::vector<VSet>();
}

BasicMemo::BasicMemo(Graph G) : AbstractMemo(G)
{
}

BasicMemo::~BasicMemo()
{
}



