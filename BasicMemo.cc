#include <vector>
#include <set>
#include <cstdlib>

#include "BasicMemo.hh"

//Ordered vertices: basic version doesn't order anything
std::vector<Vertex> BasicMemo::orderVertices(std::set<Vertex> S)
{
    std::vector<Vertex> v(S.begin(), S.end());
    return v;
}

//Basic version: upper bound is the size of the set
int BasicMemo::upperBound(std::set<Vertex> S)
{
    return S.size();
}

//Basic version: lower bound is always 1, nothing fance
int BasicMemo::lowerBound(std::set<Vertex> S)
{
    return 1;
}

//Always cache for basic memoization
bool BasicMemo::shouldCache(std::set<Vertex> S)
{
    return true;
}

//Never clean in the basic version
bool BasicMemo::needsCleaning()
{
    return false;
}

//Should never get called
std::vector<std::set<Vertex>> BasicMemo::setsToForget(std::set<Vertex> S)
{
    return std::vector<std::set<Vertex>>();
}




