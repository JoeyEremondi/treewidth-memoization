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

#include <boost/graph/dimacs.hpp>

#include <boost/timer/timer.hpp>

#include <vector>
#include <utility>
#include <ctime>

#include "memo.hh"
#include "NaiveMemo.hh"
#include "BasicMemo.hh"

#include <fstream>


using namespace boost::timer;
using namespace boost::graph;

int main()
{
    //Graph g;
    
    //boost::mt19937 rng(time (NULL));
    //boost::generate_random_graph(g, 8, 20, rng, true, true);
    
    /*
    std::ifstream inGraph;
    inGraph.open("myciel4.col");


    dimacs_basic_reader reader(inGraph, false);
    dimacs_basic_reader end;
    dimacs_edge_iterator<dimacs_basic_reader> dimacsStart(reader);
    dimacs_edge_iterator<dimacs_basic_reader> endIter(end);

    for (auto i = dimacsStart; i != endIter; i++)
    {
	std::cout << *i << std::endl;
	
    }
    

    Graph g2(dimacsStart, endIter, reader.n_vertices());
    boost::write_graphviz(std::cout, g2);
    
    
    auto iterInfo = boost::vertices(g2);
    std::set<Vertex> S(iterInfo.first, iterInfo.second);

    auto_cpu_timer* timer;

    std::cout << "Basic Memoization" << std::endl;
    timer = new auto_cpu_timer();

    auto memo2 = new BasicMemo();    
    std::cout << "Treewidth: " << memoTW(memo2, S, g2) << std::endl;
    delete memo2;
    delete timer;
    
    
    return EXIT_SUCCESS;
    */
}
