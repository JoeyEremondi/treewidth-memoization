#include "DepthBoundedMemo.hh"
#include "NaiveMemo.hh"
#include "BasicMemo.hh"

#include <set>

DepthBoundedMemo::DepthBoundedMemo(unsigned long bound)
{
    bruteForceSize = bound;
    
}


int DepthBoundedMemo::subTW(std::set<Vertex> S, Graph G)
{
    if (S.size() < bruteForceSize) //TODO different sizes?
    {
	return NaiveMemo::subTW(S, G);
    } else {
	return BasicMemo::subTW(S, G);
    }
    
    
}
