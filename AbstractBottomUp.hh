#pragma once

#include "VSet.hh"

class AbstractBottomUp
{
public:
	AbstractBottomUp(const Graph& G);
	~AbstractBottomUp();
	
protected:
	VSet S;
	int r;
	
	const Graph& G;
	int nGraph;
	int globalUpperBound;
	int twForSet(VSet S);

	virtual void beginIter() = 0;
	virtual bool iterDone() = 0;
	virtual void iterNext() = 0;

	virtual int TW(VSet S) = 0;
	virtual int updateTW(int layer, VSet S, int tw) = 0;
	virtual void beginLayer(int layer) = 0;
	virtual void endLayer(int layer) = 0;
	virtual int finalResult() = 0;
};

