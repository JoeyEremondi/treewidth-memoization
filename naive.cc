#include <cstdlib>
#include <iostream>
#include <set>
#include <climits>
#include <algorithm>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/graphviz.hpp>

#include <vector>
#include <utility>
#include <queue>

#include "naive.hh"

//using namespace std;

//Treewidth is always positive, so we can use this in place of -Inf
const int NO_WIDTH = -1;


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



int naiveTW(std::set<Vertex> S, Graph G)
{
    if (S.empty())
    {
        return NO_WIDTH;
    }
    else
    {
        int minSoFar = INT_MAX;
        
        for (auto iter = S.begin(); iter != S.end(); iter++)
        {
            Vertex v = *iter;
            std::set<Vertex> S2(S);
            
            S2.erase(v);
            
            int subTW = naiveTW(S2, G);
            int qVal = sizeQ(S2, v, G);
            
            
            
            //std::cout << "S2 is " << showSet(S2) << std::endl;
            //std::cout << "subTW " << subTW << std::endl;
            //std::cout << "qVal " << qVal << std::endl;
            
            minSoFar = std::min(minSoFar, std::max(subTW, qVal ) );
        }
        return minSoFar;
        
    }
}


/**
Functor to remove vertices in a graph which are in the given set
*/
class QFilter
{
    public:
    std::set<Vertex> S;
    QFilter() = default;
    
    QFilter(std::set<Vertex> inputSet)
    {
        S = inputSet;
    }
    
    bool operator()(const Vertex& v) const
    {
        return S.find(v) == S.end(); // keep all vertx_descriptors greater than 3
    }
};



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



int main()
{
    
    Graph g;
    boost::mt19937 rng;
    boost::generate_random_graph(g, 5, 10, rng, true, true);
    
    boost::write_graphviz(std::cout, g);
    
    
    auto iterInfo = boost::vertices(g);
    std::set<Vertex> S;
    for (auto iter = iterInfo.first; iter != iterInfo.second; iter++ )
    {
        S.insert(*iter);
    }
    
    
    std::cout << "Treewidth: " << naiveTW(S, g) << std::endl;
    return EXIT_SUCCESS;
    
}
