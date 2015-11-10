#include "ArrayOfSetBottomUp.hh"
#include <iostream>

//Initialize our heap arrays
ArrayOfSetBottomUp::ArrayOfSetBottomUp(const Graph& G) : AbstractBottomUp(G)
{
	TWarr = new std::set<VSet>*[VSet::maxNumVerts];
	upperBoundForLayer = new int[VSet::maxNumVerts];
}

//Delete our heap arrays
ArrayOfSetBottomUp::~ArrayOfSetBottomUp()
{
	delete[] TWarr;
	delete[] upperBoundForLayer;
}

//For our iteration, we need to keep track of the array index of the set we're looking at
//And an iterator for our position in that set
void ArrayOfSetBottomUp::beginIter()
{
	int prevLayer = currentLayer - 1;
	if (currentLayer > 1)
	{
		//We start looking at the sets with lowest TW values
		iterTWVal = lowerBound;
		//Find the first non-empty TW-value
		while (iterTWVal < upperBoundForLayer[prevLayer] && TWarr[prevLayer][iterTWVal].empty())
		{
			++iterTWVal;
		}

		if (iterTWVal < upperBoundForLayer[prevLayer])
		{
			//Set our iterator to the first set in that value array
			iter = TWarr[prevLayer][iterTWVal].begin();
			S = *iter;
			r = iterTWVal;

			//Cache the end of our current set
			layerEnd = TWarr[prevLayer][iterTWVal].end();
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
	int prevLayer = currentLayer - 1;
	//We're only done if we're at the end of the last value set to look at
	if (currentLayer == 1)
	{
		return haveSeenInitialElement;
	}
	if (iterTWVal >= upperBoundForLayer[prevLayer])
	{
		return true;
	}
	if (iterTWVal < upperBoundForLayer[prevLayer] - 1)
	{
		return false;
	}
	return iter == layerEnd;
}

void ArrayOfSetBottomUp::iterNext()
{
	int prevLayer = currentLayer - 1;
	//Special case, since we don't have a NO_WIDTH entry
	if (currentLayer == 1)
	{
		haveSeenInitialElement = true;
	}
	else if (iter == layerEnd)
	{
		//Find the next non-empty TW-value
		while (iterTWVal < upperBoundForLayer[prevLayer] && TWarr[prevLayer][iterTWVal].empty())
		{
			++iterTWVal;
		}
		if (iterTWVal < upperBoundForLayer[prevLayer])
		{
			//Set our iterator to the first set in that value array
			iter = TWarr[prevLayer][iterTWVal].begin();
			S = *iter;
			r = iterTWVal;

			//Cache the end of our current set
			layerEnd = TWarr[prevLayer][iterTWVal].end();
		}

	}
	else
	{
		//Delete the last element and advance the iterator
		iter = TWarr[prevLayer][iterTWVal].erase(iter);
		if (iter != layerEnd)
		{
			S = *iter;
		}

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
	return globalUpperBound;
}

void ArrayOfSetBottomUp::updateTW(int layer, VSet S, int tw)
{
	if (tw > 0)
	{
		for (int i = 0; i < tw; i++)
		{
			//Don't insert if we already have it at a lower level
			if (TWarr[layer][i].find(S) != TWarr[layer][i].end())
			{
				return;
			}
		}

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
	//Delete this layer's array of sets
	delete[] TWarr[layer];
}
//Return the lowest index of a set that contains a value in our final layer
//Otherwise, return the bound
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

//Just sum up the size sizes for each layer
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
