

#include <cstdlib>
#include <iostream>
#include <climits>
#include <algorithm>
#include <vector>
#include <utility>
#include <ctime>

#include "qset.hh"
#include "graphTypes.hh"


//Basically just a DFS
//We start at v, and search
//We expand vertices in S
//And we add to our count (without expanding) for those not in S
int sizeQ(const VSet &S, Vertex v, const Graph& G)
{
    //std::cout << "Q: starting with S = " << showSet(S) << "\n";
    
    std::vector<Vertex> open;
    open.push_back(v);
    
    VSet closed;
    
    int numInQ = 0;

    while (!open.empty() )
    {
	Vertex w = open.back();
	open.pop_back();
	auto neighbours = boost::adjacent_vertices(w,G);
	//std::cout << "Q: expanding " << w << "\n";
	
	for (auto iter = neighbours.first; iter != neighbours.second; iter++ )
	{
            Vertex u = *iter;
	    //std::cout << "Q: found neighbour " << u << "\n";
	    if (!closed.contains(u))
	    {
		//std::cout << "Q: adding " << u << " to closed\n";
		
                closed.insert(u);
		
		if (u != v && S.contains(u))
		{
		    //std::cout << "Q: adding " << u << " to queue\n";
		    open.push_back(u);
		} 
		else if (u != v)
		{
		    numInQ++;
		    
		}
	    }
	}
    }
    return numInQ;
    
}


std::string showSet(VSet S) {
    std::ostringstream result;
    
    result << "{";
        
    auto members = S.members();
    
    for (auto iter = members.begin(); iter != members.end(); iter++)
        {
            result << *iter << " ; ";
        }
        
    result << "}";
    
    return result.str();
    
}
