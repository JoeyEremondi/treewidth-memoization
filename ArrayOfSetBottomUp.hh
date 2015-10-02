#pragma once
#include "AbstractBottomUp.hh"
#include "VSet.hh"
#include "qset.hh"

class ArrayOfSetBottomUp :
	public AbstractBottomUp
{
public:
	ArrayOfSetBottomUp(const Graph& G);
	~ArrayOfSetBottomUp();

protected:
	int numUpdates = 0;
	std::set<VSet>** TWarr;
	std::set<VSet>::iterator iter;
	int iterTWVal;
	std::set<VSet>::const_iterator layerEnd;
	bool haveSeenInitialElement = false;

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

