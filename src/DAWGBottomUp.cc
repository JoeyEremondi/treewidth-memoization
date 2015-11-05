#include "DAWGBottomUp.hh"
#include <iostream>

#include <fstream>
#include <iostream>

//#define DEBUG

DAWGBottomUp::DAWGBottomUp(const Graph& G) : AbstractBottomUp(G)
{
	upperBoundForLayer = new int[VSet::maxNumVerts];
	TW.resize(VSet::maxNumVerts);
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


#ifdef DEBUG
	if (layer > 0)
	{



		//auto preTrim = TWtest[layer - 1].asDot();

		//TW[layer - 1].trim();

		//#ifdef DEBUG
		//std::ostringstream fileName, badFileName;
		//fileName << "dotOut/dawg_" << layer << ".dot";
		//badFileName << "dotOutBad/dawgBad_" << layer << ".dot";
		//std::ofstream  badGraphFile;



		//badGraphFile.open(badFileName.str());
		//badGraphFile << TW[layer - 1].asDot();
		/*
		TW[layer - 1].initIter();
		auto pair = TW[layer - 1].nextIter();
		while (!TW[layer - 1].iterDone())
		{
			VSet s = pair.first;
			int tw = pair.second;
			for (int i = 0; i < VSet::maxNumVerts; i++)
			{
				badGraphFile << s.contains(i);
			}
			badGraphFile << "(" << tw << ")\n";
			pair = TW[layer - 1].nextIter();
		}*/

		//badGraphFile.close();
		//#endif
	}
#endif



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
	//Copy the previous DAWG into this layer's entry
	//TW[layer].copyFrom(TW[layer - 1]);



}

void DAWGBottomUp::endLayer(int layer)
{

	//std::cerr << "Ending layer with " << TW[layer].numTransitions() << " transitions\n";
	TW[layer].clear();
	//std::cerr << "Called update " << numUpdates << " times\n";
	//std::cout << "TW i size: " << currentLayer << " " << numStored() << "\n";
	//TODO anything else to do here?
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
