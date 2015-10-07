#include "DAWGBottomUp.hh"
#include <iostream>
#include "AbstractMemo.hh"

//#define DEBUG

DAWGBottomUp::DAWGBottomUp(const Graph& G) : AbstractBottomUp(G)
{
	upperBoundForLayer = new int[VSet::maxNumVerts];
	//TW.resize(1);
	TW.resize(1); //Emtpy DAWG for layer 0
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
		TW[prevLayer].initIter();
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
	return TW[currentLayer - 1].iterDone();
}

void DAWGBottomUp::iterNext()
{
	if (currentLayer == 1)
	{
		haveSeenInitialElement = true;
	}


	int prevLayer = currentLayer - 1;

	auto pair = TW[prevLayer].nextIter();
	S = pair.first;
	r = pair.second;

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



	if (layer > 0)
	{

#ifdef DEBUG
		auto preTrim = TWtest[layer - 1].asDot();
#endif

		TW[layer - 1].trim();

#ifdef DEBUG
		std::cout << "Last layer wordSet real: ";
		for (auto word : TW[layer - 1].wordSet())
		{
			std::cout << "\n" << word;
		}
		std::cout << "\n\nLast layer wordSet test: ";
		for (auto word : TWtest[layer - 1].wordSet())
		{
			std::cout << "\n" << word;
		}
		std::cout << "\n";

		std::cout << "\n\n" << TW[layer - 1].asDot() << "\n\n" << TWtest[layer - 1].asDot() << "\n\n" << preTrim << "\n\n";

#endif

		//Trim the previous layer's DAWG
		//
		//Copy the previous DAWG into this layer's entry
		TW.push_back(DAWG(TW[layer - 1]));

	}

}

void DAWGBottomUp::endLayer(int layer)
{
	TW[layer].clear();
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
