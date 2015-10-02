#include "DAWGBottomUp.hh"
#include <iostream>
#include "AbstractMemo.hh"

DAWGBottomUp::DAWGBottomUp(const Graph& G) : AbstractBottomUp(G)
{
	upperBoundForLayer = new int[VSet::maxNumVerts];
	//Create our initial TW value
	//This gets repeatedly deleted and allocated
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
		//We start looking at the sets with lowest TW values
		iterTWVal = lowerBound;
		//Find the first non-empty TW-value
		while (iterTWVal < upperBoundForLayer[prevLayer] && lastLayerTW[iterTWVal].empty())
		{
			++iterTWVal;
		}

		if (iterTWVal < upperBoundForLayer[prevLayer])
		{
			//Set our iterator to the first set in that value array
			lastLayerTW[iterTWVal].initIter();
			S = lastLayerTW[iterTWVal].nextIter();
			r = iterTWVal;
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

bool DAWGBottomUp::iterDone()
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
	return lastLayerTW[iterTWVal].iterDone();
}

void DAWGBottomUp::iterNext()
{
	int prevLayer = currentLayer - 1;
	//Special case, since we don't have a NO_WIDTH entry
	if (currentLayer == 1)
	{
		haveSeenInitialElement = true;
	}
	else if (lastLayerTW[iterTWVal].iterDone())
	{
		//Find the next non-empty TW-value
		++iterTWVal;
		while (iterTWVal < upperBoundForLayer[prevLayer] && lastLayerTW[iterTWVal].empty())
		{
			++iterTWVal;
		}
		if (iterTWVal < upperBoundForLayer[prevLayer])
		{
			//Set our iterator to the first set in that value array
			lastLayerTW[iterTWVal].initIter();
			S = lastLayerTW[iterTWVal].nextIter();
			r = iterTWVal;
		}


	}
	else if (!lastLayerTW[iterTWVal].iterDone())
	{
		S = lastLayerTW[iterTWVal].nextIter();

	}
}



void DAWGBottomUp::updateTW(int layer, VSet S, int tw)
{
	if (tw > 0)
	{
		//Add it to the set for this tw value
		TW[tw].insert(S);
		//Remove it from any higher sets, if we've made an improvement
		for (int i = tw + 1; i < upperBoundForLayer[layer]; i++)
		{
			TW[i].erase(S);
		}
	}
}

void DAWGBottomUp::beginLayer(int layer)
{
	int prevLayer = layer - 1;
	int numStored = 0;

	//Generate a DAWG to store items from the last layer, erasing them as we go
	if (layer > 0)
	{
		lastLayerTW.clear();
		lastLayerTW.resize(upperBoundForLayer[prevLayer]);

		for (int i = lowerBound; i < upperBoundForLayer[prevLayer]; ++i)
		{
			//std::cout << "\n***** Starting new DAWG loop\n";
			auto loopEnd = TW[i].end();
			for (auto iter = TW[i].begin(); iter != loopEnd; iter = TW[i].erase(iter))
			{
				//std::cout << "Start layer with " << showSet(*iter) << " tw " << i <<" in prev\n";
				lastLayerTW[i].insert(*iter);
				numStored++;
				//std::cout << "Adding " << showSet(*iter) << " to DAWG\n";
				//assert(lastLayerTW[i].contains(*iter));
			}

			//Minimize our DAWG to save space
			lastLayerTW[i].minimize();

		}
	}



	//Delete the old TW to free its memory, and allocate a new one
	TW.clear();
	TW.resize(globalUpperBound);

	//Create an array entry for each possible TW value
	upperBoundForLayer[layer] = globalUpperBound;
}

void DAWGBottomUp::endLayer(int layer)
{

	//std::cout << "TW i size: " << currentLayer << " " << numStored() << "\n";
	//TODO anything else to do here?
}

int DAWGBottomUp::finalResult(int finalLayer, VSet SStart)
{
	for (int i = 0; i < globalUpperBound; i++)
	{
		if (!TW[i].empty())
		{
			return i;
		}
	}
	return globalUpperBound;
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
