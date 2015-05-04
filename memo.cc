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
#include <ctime>

#include "memo.hh"
#include "qset.hh"

//using namespace std;

//Treewidth is always positive, so we can use this in place of -Inf
const int NO_WIDTH = -1;


int memoTW(Memoizer& memo, std::set<Vertex> S, Graph G)
{
    if (S.empty())
    {
        return NO_WIDTH;
    }
    else
    {
        int minSoFar = INT_MAX;
        auto orderedVerts = memo.orderVertices(S,G);
	
        for (auto iter = orderedVerts.begin(); iter != orderedVerts.end(); iter++)
        {
            Vertex v = *iter;
            std::set<Vertex> S2(S);
            
            S2.erase(v);
            
            int subTW = memo.subTW(S2, G);
            int qVal = sizeQ(S2, v, G);

	    //Optimization: if there are more elements in the Q set than our
	    //lowest treewidth found so far, we now this vertex isn't optimal
	    //So don't recurse
	    if (qVal < minSoFar)
	    {
		minSoFar = std::min(minSoFar, std::max(subTW, qVal ) );
	    }
	    
            
            
        }
        return minSoFar;
        
    }
}





int main()
{
    
    Graph g;
    boost::mt19937 rng(time (NULL));
    boost::generate_random_graph(g, 8, 20, rng, true, true);
    
    boost::write_graphviz(std::cout, g);
    
    
    auto iterInfo = boost::vertices(g);
    std::set<Vertex> S;
    for (auto iter = iterInfo.first; iter != iterInfo.second; iter++ )
    {
        S.insert(*iter);
    }
    
    
    //std::cout << "Treewidth: " << memoTW(S, g) << std::endl;

    
    return EXIT_SUCCESS;
    
}