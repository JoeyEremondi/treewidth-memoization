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
	std::set<VSet>** TWarr;
	std::set<VSet>::iterator iter;
	int iterTWVal;
	std::set<VSet>::const_iterator layerEnd;
	bool haveSeenInitialElement = false;

	DAWG* lastLayer;

	int* upperBoundForLayer;

	void beginIter();
	bool iterDone();
	void iterNext();

	int TW(int layer, VSet S);
	void updateTW(int layer, VSet S, int tw);
	void beginLayer(int layer);
	void endLayer(int layer);
	int finalResult(int finalLayer, VSet SStart);
	int numStored();
};

