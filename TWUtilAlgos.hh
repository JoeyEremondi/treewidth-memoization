#ifndef __TW__UTIL__ALGOS__H
#define __TW__UTIL__ALGOS__H

#include "qset.hh"
#include <vector>
#include <set>

//Find the width of a given permutation
//Useful in finding heuristic upper bounds for treewidth
//TODO check if need copy?
int permutTW(std::vector<Vertex> Svec, Graph G)
{
    int tw = NO_WIDTH;
    
    while (!Svec.empty())
    {
	auto current = Svec.back();
	Svec.pop_back();
	std::set<Vertex> S(Svec.begin(), Svec.end());
	
        tw = std::max(tw, sizeQ(S, current, G));
	
    }
    return tw;

    
}


#endif
