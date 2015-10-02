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
	lastLayerTW.clear();
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
	numUpdates++;
	if (tw > 0)
	{
		
		for (int i = 0; i < tw; i++)
		{
			//Don't insert if we already have it at a lower level
			if (TW[i].find(S) != TW[i].end())
			{
				return;
			}
		}
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

		int numLastLevel = 0;
		for (int i = lowerBound; i < upperBoundForLayer[layer - 1]; i++)
		{
			numLastLevel += TW[i].size();
		}
		std::cerr << numLastLevel << " from last level in map\n";

		

		for (int i = lowerBound; i < upperBoundForLayer[prevLayer]; ++i)
		{
			int setSize = TW[i].size();

			//std::cerr << "Making DAWG for TW " << i << "\n";
			//std::cout << "\n***** Starting new DAWG loop\n";
			auto loopEnd = TW[i].end();
			
			for (auto iter = TW[i].begin(); iter != loopEnd; iter = TW[i].erase(iter))
			{
				int sizeBefore = lastLayerTW[i].size();
				std::string dotBefore = lastLayerTW[i].asDot();
				auto wordSetBefore = lastLayerTW[i].wordSet();

				//std::cout << "Start layer with " << showSet(*iter) << " tw " << i <<" in prev\n";
				lastLayerTW[i].insertSafe(*iter);
				numStored++;
				//std::cout << "Adding " << showSet(*iter) << " to DAWG\n";
				//assert(lastLayerTW[i].contains(*iter));

				int sizeAfter = lastLayerTW[i].size();
				if (sizeBefore != sizeAfter - 1)
				{
					std::cerr << "SIZE BAD AFTER INSERTION\n";

					std::cerr << "Word set before:\n\n";
					for (auto word : wordSetBefore)
					{
						std::cerr << word << "\n";
					}

					std::cerr << "\n\n\nWord set after:\n";
					for (auto word : lastLayerTW[i].wordSet())
					{
						std::cerr << word << "\n";
					}
					


					std::cerr << "\nbefore " << sizeBefore << " after " << sizeAfter << " inserted " << showSet(*iter) << "\n";
					std::cerr << "\n\n\n" << dotBefore << "\n\n\n" << lastLayerTW[i].asDot() << "\n\n\n";
					abort();
				}
			}
			

			int arrSizeBefore = lastLayerTW[i].size();
			std::string dotBefore = lastLayerTW[i].asDot();

			if (setSize != arrSizeBefore)
			{
				std::cerr << "SIZE BEFORE NOT SAME\n";
			}

			//std::cerr << "Made un-minimized DAWG\n";

			//Minimize our DAWG to save space
			lastLayerTW[i].minimize();

			int arrSizeAfter = lastLayerTW[i].size();
			if (setSize != arrSizeAfter)
			{
				std::cerr << "SIZE AFTER NOT SAME\n";
				std::cerr << "before " << arrSizeBefore << " after " << arrSizeAfter;
				std::cerr << "\n\n\n" << dotBefore << "\n\n\n" << lastLayerTW[i].asDot() << "\n\n\n";
				abort();
			}

		}

		numLastLevel = 0;
		for (int i = lowerBound; i < upperBoundForLayer[layer - 1]; i++)
		{
			numLastLevel += lastLayerTW[i].size();
		}
		std::cerr << numLastLevel << " from last level in DAWG\n";
	}



	//Delete the old TW to free its memory, and allocate a new one
	TW.clear();
	TW.resize(globalUpperBound);

	//Create an array entry for each possible TW value
	upperBoundForLayer[layer] = globalUpperBound;
}

void DAWGBottomUp::endLayer(int layer)
{
	std::cerr << "Called update " << numUpdates << " times\n";
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
