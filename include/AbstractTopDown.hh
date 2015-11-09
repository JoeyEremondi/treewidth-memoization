#pragma once

#include "qset.hh"
#include "TWUtilAlgos.hh"
#include "BottomUpTW.hh"

class AbstractTopDown
{


public:
	/*Make a new instance of the algorithm runner,
	to find the treewidth of the given graph*/
	AbstractTopDown(const Graph& G, int maxBottom);

	/*Run the algorithm and get the calculated treewidth*/
	int topDownTW();

	/*Get the value stored for the maximum bottom-up size*/
	int getMaxBottomUp();

protected:
	//How many transitions we allow our bottom-up solver to have before aborting
	int maxBottomUpSize = 1;
	const int topLevelNoStore = 0;
	const int bottomLevelNoStore = 0;

	int sharedUpperBound;
	int lowerBound;

	const Graph& G;
	int nGraph;
	std::vector<Vertex> allVertices;
	BottomUpTW bottomUpInfo;

	int numInDict;

	//std::vector<std::map<VSet, int>> TW;


	int topDownTWFromSet(const Graph& G, const VSet& S, int nSet);

	


	//Abstract methods
	virtual bool isMemoized(int nSet, VSet S) = 0;
	virtual int memoizedValue(int nSet, VSet S) = 0;
	virtual void memoizeTW(int nSet, VSet S, int tw) = 0;
	virtual void cleanMemoized() = 0;

	


};
