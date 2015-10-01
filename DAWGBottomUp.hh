#pragma once
#include "AbstractBottomUp.hh"
#include "VSet.hh"
#include "qset.hh"
#include "DAWG.hh"

class DAWGBottomUp :
	public AbstractBottomUp
{
public:
	DAWGBottomUp(const Graph& G);
	~DAWGBottomUp();

protected:
	std::vector<std::set<VSet>> TW;
	int iterTWVal = 0;
	bool haveSeenInitialElement = false;

	std::vector<DAWG> lastLayerTW;

	int* upperBoundForLayer;

	void beginIter();
	bool iterDone();
	void iterNext();

	void updateTW(int layer, VSet S, int tw);
	void beginLayer(int layer);
	void endLayer(int layer);
	int finalResult(int finalLayer, VSet SStart);
	int numStored();
};

