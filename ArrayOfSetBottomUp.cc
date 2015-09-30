#include "ArrayOfSetBottomUp.hh"
#include <iostream>
#include "AbstractMemo.hh"

ArrayOfSetBottomUp::ArrayOfSetBottomUp(const Graph& G) : AbstractBottomUp(G)
{
	TWarr = new std::set<VSet>*[VSet::maxNumVerts];
	upperBoundForLayer = new int[VSet::maxNumVerts];
}


ArrayOfSetBottomUp::~ArrayOfSetBottomUp()
{
	delete[] TWarr;
	delete[] upperBoundForLayer;
}

void ArrayOfSetBottomUp::beginIter()
{
	if (currentLayer > 1)
	{
		//We start looking at the sets with lowest TW values
		iterTWVal = lowerBound;
		//Find the first non-empty TW-value
		while (TWarr[currentLayer][iterTWVal].empty())
		{
			++iterTWVal;
		}

		if (iterTWVal < upperBoundForLayer[currentLayer])
		{
			//Set our iterator to the first set in that value array
			iter = TWarr[currentLayer][iterTWVal].begin();
			S = *iter;
			r = iterTWVal;

			//Cache the end of our current set
			layerEnd = TWarr[currentLayer][iterTWVal].end();
		}
	}
	else
	{
		//Special case for layer 1: empty set and no TW
		VSet SEmpty;
		S = SEmpty;
		r = NO_WIDTH;
	}

}

bool ArrayOfSetBottomUp::iterDone()
{
	//We're only done if we're at the end of the last value set to look at
	if (currentLayer == 1)
	{
		return haveSeenInitialElement;
	}
	if (iterTWVal > upperBoundForLayer[currentLayer])
	{
		return true;
	}
	if (iterTWVal < upperBoundForLayer[currentLayer] - 1)
	{
		return false;
	}
	return iter == layerEnd;
}

void ArrayOfSetBottomUp::iterNext()
{
	//Special case, since we don't have a NO_WIDTH entry
	if (currentLayer == 1)
	{
		haveSeenInitialElement = true;
	}
	else if (iter == layerEnd)
	{
		//Find the next non-empty TW-value
		while (TWarr[currentLayer][iterTWVal].empty())
		{
			++iterTWVal;
		}
		if (iterTWVal < upperBoundForLayer[currentLayer])
		{
			//Set our iterator to the first set in that value array
			iter = TWarr[currentLayer][iterTWVal].begin();
			S = *iter;
			r = iterTWVal;

			//Cache the end of our current set
			layerEnd = TWarr[currentLayer][iterTWVal].end();
		}

	}
	else
	{
		//Delete the last element and advance the iterator
		iter = TWarr[currentLayer][iterTWVal].erase(iter);
		S = *iter;
	}

}

int ArrayOfSetBottomUp::TW(int layer, VSet S)
{
	for (int i = lowerBound; i < upperBoundForLayer[layer]; i++)
	{
		auto searchInfo = TWarr[layer][i].find(S);
		if (searchInfo != TWarr[layer][i].end())
		{
			return i;
		}
	}
	//TODO should never hit this?
	std::cerr << "ERROR! Should never not find cached TW!"
	return globalUpperBound;
}

void ArrayOfSetBottomUp::updateTW(int layer, VSet S, int tw)
{
	if (tw > 0)
	{
		//Add it to the set for this tw value
		TWarr[layer][tw].insert(S);
		//Remove it from any higher sets, if we've made an improvement
		for (int i = tw + 1; i < upperBoundForLayer[layer]; i++)
		{
			TWarr[layer][i].erase(S);
		}
	}
}

void ArrayOfSetBottomUp::beginLayer(int layer)
{
	//Create an array entry for each possible TW value
	upperBoundForLayer[layer] = globalUpperBound;
	TWarr[layer] = new std::set<VSet>[globalUpperBound];
}

void ArrayOfSetBottomUp::endLayer(int layer)
{
	std::cout << "TW i size: " << currentLayer << " " << numStored() << "\n";
	//Delete this layer's array of sets
	delete[] TWarr[layer];
}

int ArrayOfSetBottomUp::finalResult(int finalLayer, VSet SStart)
{
	for (int i = 0; i < globalUpperBound; i++)
	{
		if (!TWarr[finalLayer][i].empty())
		{
			return i;
		}
	}
	return globalUpperBound;
}

int ArrayOfSetBottomUp::numStored()
{
	int ret = 0;
	//Look at all values stored in this layer and the last
	for (int i = lowerBound; i < upperBoundForLayer[currentLayer]; i++)
	{
		ret += TWarr[currentLayer][i].size();
	}

	for (int i = lowerBound; i < upperBoundForLayer[currentLayer - 1]; i++)
	{
		ret += TWarr[currentLayer - 1][i].size();
	}

	return ret;
}
