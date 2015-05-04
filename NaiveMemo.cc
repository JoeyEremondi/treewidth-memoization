#include <vector>
#include <set>
#include <cstdlib>

#include "NaiveMemo.hh"
#include "qset.hh"

    //We just recursively call our TW algorithm in the naive version
int NaiveMemo::subTW(std::set<Vertex> S, Graph G) 
{
    return memoTW(this, S, G);
}


    //Ordered vertices: naive version doesn't order anything
std::vector<Vertex> NaiveMemo::orderVertices(std::set<Vertex> S, Graph G)
{
    std::vector<Vertex> v(S.begin(), S.end());
    return v;
}
