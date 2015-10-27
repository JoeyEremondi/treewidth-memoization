#include <cstdlib>
#include <iostream>
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

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <vector>
#include <utility>
#include <ctime>

#include "AbstractMemo.hh"
#include "DepthBoundedMemo.hh"
#include "SimplicialFirstMemo.hh"
#include "BasicMemo.hh"

#include "TreeBottomUp.hh"
#include "ArrayOfSetBottomUp.hh"
#include "DAWGBottomUp.hh"

#include "LastInsertedTopDown.hh"

#include "HeuristicUpperBoundMemo.hh"

#include <fstream>

#include <sstream>

#include "DIMACS.hh"
//#include "BottomUpTW.hh"
#include "TopDownTW.hh"

#include "qset.hh"

using namespace boost::timer;
using namespace boost::graph;

int main(int argc, char** argv)
{

	/*
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
	} */


	std::vector<std::string> filesInDir;
	//({"myciel3.dgf" , "pathfinder-pp.dgf" ,  "myciel4.dgf" , "queen5_5.dgf" , "queen6_6.dgf"  , "queen7_7.dgf"  });

	auto dirIter = boost::filesystem::directory_iterator("testGraphs");
	auto dirIterEnd = boost::filesystem::directory_iterator();

	for (auto graphFile = dirIter; graphFile != dirIterEnd; ++graphFile)
	{
		auto fileName = graphFile->path().filename().string();
		if (graphFile->path().extension() == ".dgf")
		{
			filesInDir.push_back(fileName);
		}

	}

	//separate variable makes switching for testing easier
	std::vector<std::string>& inFiles = filesInDir;

	if (argc > 1)
	{
		inFiles = { argv[1] };
	}

	//std::vector<std::string> inFiles({"eil51.tsp.dgf" });
	//std::vector<std::string> inFiles({ "queen7_7.dgf" });
	//std::vector<std::string> inFiles({ "mildew.dgf" });
	//std::vector<std::string> inFiles({ "myciel4.dgf" });
	//std::vector<std::string> inFiles({ "oesoca42.dgf" });

	//std::vector<std::string> inFiles({ "queen5_5.dgf", "queen6_6.dgf", "myciel3.dgf", "myciel4.dgf" });



	for (auto inFile : inFiles)
	{

		std::ifstream inGraphFile;
		inGraphFile.open("testGraphs/" + inFile);
		std::cout << "\n\n***********\nTesting on graph " << inFile << "\n***********\n";

		//std::ofstream outGraph;
		//outGraph.open("testGraphs/DOT/" + *iter + ".dot");

		Graph testGraph;

		read_coloring_problem(inGraphFile, testGraph);
		//boost::write_graphviz(outGraph, gRand);
		inGraphFile.close();

		//TODO better place for this?
		VSet::maxNumVerts = boost::num_vertices(testGraph);

		std::cout << "Num Vertices: " << boost::num_vertices(testGraph) << "\n";
		std::cout << "Num Edges: " << boost::num_edges(testGraph) << "\n\n";

		//boost::generate_random_graph(gRand, vc, ec, rng, true, true);


		//try {
			auto_cpu_timer* timer;

			//std::cout << "Set based Bottom-up Memoization" << std::endl;
			//timer = new auto_cpu_timer();
			//ArrayOfSetBottomUp bottomUpSearcher(testGraph);
			//std::cout << "Treewidth: " << bottomUpSearcher.tw() << "\n\n\n";
			//delete timer;

			std::cout << "DAWG based Bottom-up Memoization" << std::endl;
			timer = new auto_cpu_timer();
			DAWGBottomUp dawgSearcher(testGraph);
			std::cout << "Treewidth: " << dawgSearcher.tw() << "\n\n\n";
			delete timer;

			//std::cout << "Top Down" << std::endl;
			//timer = new auto_cpu_timer();
			//LastInsertedTopDown tdtw(testGraph);
			//std::cout << "Treewidth: " << tdtw.topDownTW() << "\n\n\n";
			//delete timer;

			/*
			std::cout << "Simplicial-first Memoization" << std::endl;
			timer = new auto_cpu_timer();
			auto memo4 = new SimplicialFirstMemo(gRand);
			std::cout << "Treewidth: " << memo4->treeWidth() << std::endl;
			memo4->printStats();
			delete memo4;
			delete timer;



			std::cout << "Basic Memoization" << std::endl;
			timer = new auto_cpu_timer();
			auto memo2 = new BasicMemo(gRand);
			std::cout << "Treewidth: " << memo2->treeWidth() << std::endl;
			memo2->printStats();
			delete memo2;
			delete timer;



			std::cout << "Depth-bounded Memoization" << std::endl;
			timer = new auto_cpu_timer();

			auto memo3 = new DepthBoundedMemo(5, gRand);
			std::cout << "Treewidth: " << memo3->treeWidth() << std::endl;
			memo3->printStats();
			delete memo3;
			delete timer;



			std::cout << "Heuristic UB Memoization" << std::endl;
			timer = new auto_cpu_timer();
			auto memo5 = new HeuristicUpperBoundMemo(gRand);
			std::cout << "Treewidth: " << memo5->treeWidth() << std::endl;
			memo5->printStats();
			delete memo5;
			delete timer;
			*/
		/*}
		catch (const std::bad_alloc& e) {
			std::cerr << "Allocation failed in main: " << e.what() << '\n';
			//return -1;
		}*/


	}




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
