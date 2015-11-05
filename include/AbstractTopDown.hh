#pragma once

#include "qset.hh"
#include "TWUtilAlgos.hh"
#include "BottomUpTW.hh"

class AbstractTopDown
{
protected:
	const int maxDictSize = 1000000000;
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

	AbstractTopDown(const Graph& G);

public:
	static const int maxBottumUpSize = 1;
	int topDownTW();

	//TODO remove
	int numSimplicial = 0;
	int improvedByLocalLower = 0;
	int numPruned;


};
