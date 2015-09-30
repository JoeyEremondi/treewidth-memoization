#pragma once

#include "VSet.hh"

class AbstractBottomUp
{
public:
	AbstractBottomUp(const Graph& G);
	~AbstractBottomUp();
	int tw();
	
protected:
	VSet S;
	int r;
	
	const Graph& G;
	int nGraph;
	int globalUpperBound;
	int twForSet(VSet S);
	int currentLayer = 0;
	int lowerBound;

	virtual void beginIter() = 0;
	virtual bool iterDone() = 0;
	virtual void iterNext() = 0;

	virtual int TW(int layer, VSet S) = 0;
	virtual void updateTW(int layer, VSet S, int tw) = 0;
	virtual void beginLayer(int layer) = 0;
	virtual void endLayer(int layer) = 0;
	virtual int finalResult(int finalLayer, VSet SStart) = 0;
	virtual int numStored() = 0;
};

