#include <cstdlib>
#include <iostream>
#include <set>
#include <climits>
#include <algorithm>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/graph/graphviz.hpp>

#include <vector>
#include <utility>
#include <queue>
#include <ctime>

#include "naive.hh"

//using namespace std;

//Treewidth is always positive, so we can use this in place of -Inf
const int NO_WIDTH = -1;



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



int naiveMain()
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
    
    
    std::cout << "Treewidth: " << naiveTW(S, g) << std::endl;

    
    return EXIT_SUCCESS;
    
}
