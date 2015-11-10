#include <iostream>

#include <boost/timer/timer.hpp>

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <vector>

#include "ArrayOfSetBottomUp.hh"
#include "DAWGBottomUp.hh"
#include "LastInsertedTopDown.hh"

#include <fstream>

#include <sstream>

#include "DIMACS.hh"

using namespace boost::timer;
using namespace boost::graph;

int main(int argc, char** argv)
{


	std::vector<std::string> filesInDir;

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


	for (auto inFile : inFiles)
	{

		std::ifstream inGraphFile;
		inGraphFile.open("testGraphs/" + inFile);
		std::cout << "\n\n***********\nTesting on graph " << inFile << "\n***********\n";


		Graph testGraph;

		read_coloring_problem(inGraphFile, testGraph);
		inGraphFile.close();

		//Set our vSet max number of verts globally here, speeds up vSet creation
		VSet::maxNumVerts = boost::num_vertices(testGraph);

		std::cout << "Num Vertices: " << boost::num_vertices(testGraph) << "\n";
		std::cout << "Num Edges: " << boost::num_edges(testGraph) << "\n\n";

		auto_cpu_timer* timer;

		std::cout << "Set based Bottom-up Memoization" << std::endl;
		timer = new auto_cpu_timer();
		ArrayOfSetBottomUp bottomUpSearcher(testGraph);
		std::cout << "Treewidth: " << bottomUpSearcher.tw() << "\n\n\n";
		delete timer;

		std::cout << "DAWG based Bottom-up Memoization, max " << DAWG::ABS_MAX_TRANSITIONS << std::endl;
		timer = new auto_cpu_timer();
		DAWGBottomUp dawgSearcher(testGraph);
		std::cout << "Treewidth: " << dawgSearcher.tw() << "\n\n\n";
		delete timer;


		timer = new auto_cpu_timer();
		LastInsertedTopDown tdtwHybrid(testGraph, 10000, 9999991, 100);
		std::cout << "Top Down " << tdtwHybrid.getMaxBottomUp() << std::endl;
		std::cout << "Treewidth: " << tdtwHybrid.topDownTW() << "\n\n\n";
		delete timer;

		timer = new auto_cpu_timer();
		LastInsertedTopDown tdtw(testGraph, 1, 9999991, 100);
		std::cout << "Top Down " << tdtw.getMaxBottomUp() << std::endl;
		std::cout << "Treewidth: " << tdtw.topDownTW() << "\n\n\n";
		delete timer;


	}


	return EXIT_SUCCESS;
}
