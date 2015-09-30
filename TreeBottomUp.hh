#pragma once
#include "AbstractBottomUp.hh"
class TreeBottomUp :
	public AbstractBottomUp
{
public:
	TreeBottomUp(const Graph& G);
	~TreeBottomUp();

protected:
	std::map<VSet, int>** TWarr;
	std::map<VSet, int>::iterator iter;
	std::map<VSet, int>::iterator layerEnd;

	void beginIter();
	bool iterDone();
	void iterNext();

	int TW(int layer, VSet S);
	void updateTW(int layer, VSet S, int tw);
	void beginLayer(int layer);
	void endLayer(int layer);
	int finalResult(int finalLayer, VSet SStart);
};

