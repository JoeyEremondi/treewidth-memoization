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
#include <ctime>

#include "memo.hh"
#include "NaiveMemo.hh"


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
    
    auto memo = new NaiveMemo();
    
    std::cout << "Treewidth: " << memoTW(memo, S, g) << std::endl;

    delete memo;
    
    return EXIT_SUCCESS;
    
}
