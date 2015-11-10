#include "BottomUpTW.hh"

#include "UpperBound.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream
#include <cassert>
#include <boost/unordered_map.hpp>


BottomUpTW::BottomUpTW(const Graph & gIn, int maxLayerSizeIn)
	: G(gIn)
{
	maxLayerSize = maxLayerSizeIn;

	VSet S(gIn);

	//Remove elements in the max-clique of G
	VSet maxClique = exactMaxClique(gIn);
	std::vector<Vertex> cliqueVec(maxClique.size());
	maxClique.members(cliqueVec);

	for (auto iter = cliqueVec.begin(); iter != cliqueVec.end(); ++iter)
	{
		S.erase(*iter);
	}

	upperBound = calcUpperBound(G, S);


	this->fillTWSet(S);

}

void BottomUpTW::fillTWSet(const VSet& SStart)
{
	auto TW = new std::map<VSet, int>*[VSet::maxNumVerts];

	VSet emptySet;
	TW[0] = new std::map<VSet, int>();
	(*TW[0])[emptySet] = NO_WIDTH;

	//Initial values for our data, in case we never succeed
	this->didFindSolution = false;
	this->twValueFound = NO_WIDTH;
	this->maxLevelReached = -1;
	this->topLevelDictFound = NULL;



	int nGraph = boost::num_vertices(G);
	int nSet = SStart.size();


	std::vector<Vertex> vertInfo;

	auto allVertInfo = boost::vertices(G);
	std::vector<Vertex> allVerts(allVertInfo.first, allVertInfo.second);


	SStart.members(vertInfo);
	auto vertInfoStart = vertInfo.begin();
	auto vertInfoEnd = vertInfo.end();

	int numStoredElements = 0;

	for (int i = 1; i <= nSet; ++i)
	{
		try {
			//Make sure our dict doesn't get too big
			numStoredElements = 0;

			//Initialize our dictionary at this level
			TW[i] = new std::map<VSet, int>();

			int minTW = upperBound;

			//Store the |Q| values, overwritten each layer
			std::vector<int> qSizes(nGraph);

			auto twLoopEnd = TW[i - 1]->end();
			for (auto pair = TW[i - 1]->begin(); pair != twLoopEnd; ++pair)
			{
				VSet S = pair->first;
				int r = pair->second;

				if (r < upperBound)
				{

					Vertex firstSet = S.first();

					findQvalues(nGraph, S, G, qSizes);

					for (auto x = vertInfoStart; x != vertInfoEnd; ++x)
					{
						Vertex v = *x;
						if ((!S.contains(v)))
						{



							VSet SUx = S;
							SUx.insert(v);

							int q = qSizes[v];


							int rr = std::max(r, q);

							minTW = std::min(minTW, rr);
							upperBound = std::min(upperBound, std::max(minTW, nGraph - i - 1));

							if (rr < upperBound)
							{
								auto searchInfo = TW[i]->find(SUx);
								if (searchInfo == TW[i]->end())
								{
									//Create new element in our of stored values, if we have space
									if (numStoredElements < maxLayerSize)
									{
										(*TW[i])[SUx] = rr;
										numStoredElements++;
									}
									else //If we run out of space, store the last dict we fully calculated
									{
										this->didFindSolution = false;
										this->twValueFound = NO_WIDTH;
										this->maxLevelReached = i - 1;
										this->topLevelDictFound = TW[i - 1];
										delete TW[i];
										delete TW;
										return;
									}

								}
								else if (rr < searchInfo->second)
								{
									(*TW[i])[SUx] = rr;
								}


							}
						}

					}
				}



			}

			//De-allocate our old level of the tree, to save space
			delete TW[i - 1];
		}
		catch (const std::bad_alloc& e) {
			std::cerr << "Level " << i << ", num in Dict " << numStoredElements << "\n";
			std::cerr << "allocation failed in BUTW: " << e.what() << '\n';

			//Keep going, like we hit our limit

			this->didFindSolution = false;
			this->twValueFound = NO_WIDTH;
			this->maxLevelReached = i - 1;
			this->topLevelDictFound = TW[i - 1];
			delete TW[i];
			delete TW;
			return;
		}

	}

	//If we got to this point without failing, then we found a solution
	didFindSolution = true;

	auto searchInfo = TW[nSet]->find(SStart);
	if (searchInfo == TW[nSet]->end())
	{
		twValueFound = upperBound;
	}
	else
	{
		twValueFound = searchInfo->second;
	}
	//Delete our array, and the last layer, we don't need it if we found a solution
	delete TW[nSet];
	delete[] TW;



}
