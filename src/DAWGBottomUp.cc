#include "DAWGBottomUp.hh"
#include <iostream>

#include <fstream>
#include <iostream>

DAWGBottomUp::DAWGBottomUp(const Graph& G) : AbstractBottomUp(G)
{
	upperBoundForLayer = new int[VSet::maxNumVerts];
	TW.resize(VSet::maxNumVerts);
}


DAWGBottomUp::~DAWGBottomUp()
{
	TW.clear();
	delete[] upperBoundForLayer;
}

void DAWGBottomUp::beginIter()
{
	int prevLayer = currentLayer - 1;
	if (currentLayer > 1)
	{
		currentIter = TW[prevLayer].begin();
		//set the first elements after we've done our setup
		this->iterNext();

	}
	else
	{
		//Special case for layer 1: empty set and no TW
		VSet SEmpty;
		S = SEmpty;
		r = NO_WIDTH;
	}

}

bool DAWGBottomUp::iterDone()
{
	if (currentLayer == 1)
	{
		return haveSeenInitialElement;
	}
	return currentIter == TW[currentLayer - 1].end();
}

void DAWGBottomUp::iterNext()
{
	if (currentLayer == 1)
	{
		haveSeenInitialElement = true;
	}


	int prevLayer = currentLayer - 1;

	++currentIter;
	S = currentIter->first;
	r = currentIter->second;

}



void DAWGBottomUp::updateTW(int layer, VSet S, int tw)
{
	numUpdates++;
	if (tw > 0)
	{

		TW[layer].insert(S, tw);
	}
}

void DAWGBottomUp::beginLayer(int layer)
{
	//int prevLayer = layer - 1;
	int numStored = 0;

	//Create an array entry for each possible TW value
	upperBoundForLayer[layer] = globalUpperBound;


}

void DAWGBottomUp::endLayer(int layer)
{
	TW[layer].clear();
}

int DAWGBottomUp::finalResult(int finalLayer, VSet SStart)
{
	int twVal = TW[finalLayer].contains(SStart);
	if (twVal == TW[finalLayer].NOT_CONTAINED)
	{
		return globalUpperBound;
	}
	return twVal;
}

int DAWGBottomUp::numStored()
{
	int ret = 0;
	//Look at all values stored in this layer and the last
	for (int i = lowerBound; i < upperBoundForLayer[currentLayer]; i++)
	{
		ret += TW[i].size();
	}

	return ret;
}
