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

#include <boost/timer/timer.hpp>

#include <vector>
#include <utility>
#include <ctime>

#include "memo.hh"
#include "NaiveMemo.hh"
#include "BasicMemo.hh"

using namespace boost::timer;

int main()
{
    
    Graph g;
    boost::mt19937 rng(time (NULL));
    boost::generate_random_graph(g, 8, 20, rng, true, true);
    
    boost::write_graphviz(std::cout, g);
    
    
    auto iterInfo = boost::vertices(g);
    std::set<Vertex> S(iterInfo.first, iterInfo.second);

    auto_cpu_timer* timer;

    std::cout << "Naive version" << std::endl;
    timer = new auto_cpu_timer();

    auto memo = new NaiveMemo();    
    std::cout << "Treewidth: " << memoTW(memo, S, g) << std::endl;
    delete memo;
    delete timer;

    std::cout << "Basic Memoization" << std::endl;
    timer = new auto_cpu_timer();

    BasicMemo* memo2 = new BasicMemo();    
    std::cout << "Treewidth: " << memoTW(memo2, S, g) << std::endl;
    delete memo;
    delete timer;
    
    return EXIT_SUCCESS;
    
}
