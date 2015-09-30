#include "TreeBottomUp.hh"



TreeBottomUp::TreeBottomUp(const Graph& G) : AbstractBottomUp(G)
{
	TWarr = new std::map<VSet, int>*[VSet::maxNumVerts];
}


TreeBottomUp::~TreeBottomUp()
{
	delete[] TWarr;
}

void TreeBottomUp::beginIter()
{
	iter = TWarr[currentLayer-1]->begin();
	layerEnd = TWarr[currentLayer-1]->end();
	S = iter->first;
	r = iter->second;
}

bool TreeBottomUp::iterDone()
{
	return iter == layerEnd;
}

void TreeBottomUp::iterNext()
{
	iter++;
	if (iter != layerEnd)
	{
		S = iter->first;
		r = iter->second;
	}
}

int TreeBottomUp::TW(int layer, VSet S)
{
	return (*TWarr[layer])[S];
}

void TreeBottomUp::updateTW(int layer, VSet S, int tw)
{
	auto searchInfo = (*TWarr[layer]).find(S);
	if (searchInfo == (*TWarr[layer]).end())
	{
		(*TWarr[layer])[S] = tw;
	}
	else
	{
		(*TWarr[layer])[S] = std::min(searchInfo->second, tw);
	}
}

void TreeBottomUp::beginLayer(int layer)
{
	TWarr[layer] = new std::map<VSet, int>();
}

void TreeBottomUp::endLayer(int layer)
{
	delete TWarr[layer];
}

int TreeBottomUp::finalResult(int finalLayer, VSet SStart)
{
	auto searchInfo = TWarr[finalLayer]->find(SStart);
	if (searchInfo == TWarr[finalLayer]->end())
	{
		return globalUpperBound;
	}
	return searchInfo->second;
}


