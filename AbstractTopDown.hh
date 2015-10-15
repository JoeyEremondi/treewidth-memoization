#pragma once

#include "qset.hh"
#include "TWUtilAlgos.hh"
#include "AbstractMemo.hh"
#include "BottomUpTW.hh"

class AbstractTopDown
{
protected:
	const int maxDictSize = 1000000000;
	const int maxBottumUpSize = 1000000000;
	const int topLevelNoStore = 10;
	const int bottomLevelNoStore = 5;

	int sharedUpperBound;
	int lowerBound;
	int numInDict;

	const Graph& G;
	int nGraph;
	std::vector<Vertex> allVertices;
	BottomUpTW bottomUpInfo;

	//std::vector<std::map<VSet, int>> TW;

	int topDownTW(const Graph& G);
	int topDownTWFromSet(const Graph& G, const VSet& S, int nSet);


	//Abstract methods
	virtual bool isMemoized(int nSet, VSet S) = 0;
	virtual int memoizedValue(int nSet, VSet S) = 0;
	virtual void memoizeTW(int nSet, VSet S, int tw) = 0;
	virtual void cleanMemoized() = 0;

	AbstractTopDown(const Graph& G);


};