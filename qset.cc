#include "qset.hh"

#include <cstdlib>
#include <iostream>
#include <set>
#include <climits>
#include <algorithm>
#include <vector>
#include <utility>
#include <queue>
#include <ctime>

//Basically just a BFS
//We start at v, and search
//We expand vertices in S
//And we add to our count (without expanding) for those not in S
int sizeQ(std::set<Vertex> S, Vertex v, Graph G)
{
    std::queue<Vertex> open;
    open.push(v);
    
    std::set<Vertex> closed;
    
    int numInQ = 0;

    while (!open.empty() )
    {
	Vertex w = open.front();
	open.pop();
	auto neighbours = boost::adjacent_vertices(w,G);
	for (auto iter = neighbours.first; iter != neighbours.second; iter++ )
	{
            Vertex u = *iter;
	    if (closed.find(u) == closed.end())
	    {
		bool inS = S.find(u) != S.end();
                closed.insert(u);
		
		if (u != v && !inS)
		{
		    open.push(u);
		} 
		else 
		{
		    numInQ++;
		    
		}
	    }
	}
    }
    return numInQ;
    
}


std::string showSet(std::set<Vertex> S) {
    std::ostringstream result;
    
    result << "{";
        
        
        for (auto iter = S.begin(); iter != S.end(); iter++)
        {
            result << *iter << " ; ";
        }
        
    result << "}";
    
    return result.str();
    
}
