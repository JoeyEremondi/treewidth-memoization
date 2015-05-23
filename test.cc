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

#include "AbstractMemo.hh"
#include "DepthBoundedMemo.hh"
#include "SimplicialFirstMemo.hh"
#include "BasicMemo.hh"

#include "HeuristicUpperBoundMemo.hh"

#include <fstream>

#include <sstream>


using namespace boost::timer;
using namespace boost::graph;

int main(int argc, char** argv)
{

    //Get our vertex and edge counts from the command line
    std::istringstream vcs(argv[1]);
    std::istringstream ecs(argv[2]);

    int vc, ec;
    
    if (!(vcs >> vc))
    {
        std::cerr << "Invalid number " << argv[1] << '\n';
	return -1;
    }
    if (!(ecs >> ec))
    {
        std::cerr << "Invalid number " << argv[2] << '\n';
	return -1;
    }
    	



    Graph gRand;
    
    boost::mt19937 rng(time (NULL));
    boost::generate_random_graph(gRand, vc, ec, rng, true, true);

    auto_cpu_timer* timer;

    /*
    std::cout << "Basic Memoization" << std::endl;
    timer = new auto_cpu_timer();

    
    auto memo2 = new BasicMemo(gRand);    
    std::cout << "Treewidth: " << memo2->treeWidth() << std::endl;
    delete memo2;
    delete timer;

    std::cout << "Depth-bounded Memoization" << std::endl;
    timer = new auto_cpu_timer();

    auto memo3 = new DepthBoundedMemo(5, gRand);    
    std::cout << "Treewidth: " << memo3->treeWidth() << std::endl;
    delete memo3;
    delete timer;

    std::cout << "Simplicial-first Memoization" << std::endl;
    timer = new auto_cpu_timer();
    auto memo4 = new SimplicialFirstMemo(gRand);    
    std::cout << "Treewidth: " << memo4->treeWidth() << std::endl;
    delete memo4;
    delete timer; */

    std::cout << "Heuristic UB Memoization" << std::endl;
    timer = new auto_cpu_timer();
    auto memo5 = new HeuristicUpperBoundMemo(gRand);    
    std::cout << "Treewidth: " << memo5->treeWidth() << std::endl;
    delete memo5;
    delete timer;


    //boost::write_graphviz(std::cout, gRand);
    
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
