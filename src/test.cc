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

/*
Test file for treewidth methods

USAGE:
  ./test # runs all tests on all files in ./testGraphs
  ./test testToRun # runs given test tests on all files in ./testGraphs
  ./test testToRun inFile # runs the given test on the given .dgf graph file

  Possible tests to run:
	all: run all tests
	dawg100m : DAWG with staging area size 100 million
	dawg50m : DAWG with staging area size 50 million
	dawg10m : DAWG with staging area size  10 million
	tree : bottom-up method with arrays of std::set (trees)
	topDown : Top-down method
	hybrid: bottom up with limit of 10000, then switch to top-down
*/
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

	std::vector<std::string> inFiles;

	//Process our arguments
	for (int i = 1; i < argc; i++)
	{
		if (argv[i])
		{

		}
	}

	std::string testToRun;

	//No args given: we run all tests, on all graphs in the /testGraphs folder
	if (argc == 1)
	{
		testToRun = "all";
		for (auto inFile : filesInDir)
		{
			inFiles.push_back("testGraphs/" + inFile);
		}
	}
	//One arg given: do all in testGraphs, and run all tests on them
	else if (argc == 2)
	{
		testToRun = argv[1];
		for (auto inFile : filesInDir)
		{
			inFiles.push_back("testGraphs/" + inFile);
		}

	}
	//Otherwise: use given test and input file
	else if (argc > 2)
	{
		testToRun = argv[1];
		inFiles = { argv[2] };
	}
	else
	{
		std::cerr << "Invalid number of arguments\n";
		abort();
	}




	for (auto inFile : inFiles)
	{

		std::ifstream inGraphFile;
		inGraphFile.open(inFile);
		std::cout << "\n\n***********\nTesting on graph " << inFile << "\n***********\n";


		Graph testGraph;

		read_coloring_problem(inGraphFile, testGraph);
		inGraphFile.close();

		//Set our vSet max number of verts globally here, speeds up vSet creation
		VSet::maxNumVerts = boost::num_vertices(testGraph);

		std::cout << "Num Vertices: " << boost::num_vertices(testGraph) << "\n";
		std::cout << "Num Edges: " << boost::num_edges(testGraph) << "\n\n";

		auto_cpu_timer* timer;

		if (testToRun == "all" || testToRun == "tree")
		{
			std::cout << "Set based Bottom-up Memoization" << std::endl;
			timer = new auto_cpu_timer();
			ArrayOfSetBottomUp bottomUpSearcher(testGraph);
			std::cout << "Treewidth: " << bottomUpSearcher.tw() << "\n\n\n";
			delete timer;
		}

		if (testToRun == "all" || testToRun == "dawg100M")
		{
			DAWGBottomUp dawgSearcher(testGraph, 100 * 1000000);
			std::cout << "DAWG based Bottom-up Memoization, max " << dawgSearcher.maxTransitions() << std::endl;
			timer = new auto_cpu_timer();
			std::cout << "Treewidth: " << dawgSearcher.tw() << "\n\n\n";
			delete timer;
		}

		if (testToRun == "all" || testToRun == "dawg50M")
		{
			DAWGBottomUp dawgSearcher(testGraph, 50 * 1000000);
			std::cout << "DAWG based Bottom-up Memoization, max " << dawgSearcher.maxTransitions() << std::endl;
			timer = new auto_cpu_timer();
			std::cout << "Treewidth: " << dawgSearcher.tw() << "\n\n\n";
			delete timer;
		}

		if (testToRun == "all" || testToRun == "dawg10M")
		{
			DAWGBottomUp dawgSearcher(testGraph, 10 * 1000000);
			std::cout << "DAWG based Bottom-up Memoization, max " << dawgSearcher.maxTransitions() << std::endl;
			timer = new auto_cpu_timer();
			std::cout << "Treewidth: " << dawgSearcher.tw() << "\n\n\n";
			delete timer;
		}

		if (testToRun == "all" || testToRun == "topDown")
		{
			timer = new auto_cpu_timer();
			auto x = boost::vertices(testGraph).second;
			LastInsertedTopDown tdtwHybrid(testGraph, 10000, 9999991, 100);
			std::cout << "Top Down hybrid, bottom-up limit of " << tdtwHybrid.getMaxBottomUp() << std::endl;
			std::cout << "Treewidth: " << tdtwHybrid.topDownTW() << "\n\n\n";
			delete timer;
		}

		if (testToRun == "all" || testToRun == "hybrid")
		{
			timer = new auto_cpu_timer();
			LastInsertedTopDown tdtw(testGraph, 1, 9999991, 100);
			std::cout << "Top Down " << tdtw.getMaxBottomUp() << std::endl;
			std::cout << "Treewidth: " << tdtw.topDownTW() << "\n\n\n";
			delete timer;
		}


	}


	return EXIT_SUCCESS;
}
