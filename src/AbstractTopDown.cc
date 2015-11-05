#include "AbstractTopDown.hh"

#include "UpperBound.hh"
#include "BottomUpTW.hh"

#include "Simplicial.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream
#include <cassert>



AbstractTopDown::AbstractTopDown(const Graph& gIn)
	: G(gIn)
	, allVertices(boost::vertices(G).first, boost::vertices(G).second)
	, bottomUpInfo(G, maxBottumUpSize)
{
	nGraph = boost::num_vertices(G);
	std::sort(allVertices.begin(), allVertices.end(),
		[gIn](auto v1, auto v2) -> bool
	{
		return boost::degree(v1, gIn) > boost::degree(v2, gIn);
	});

}

int AbstractTopDown::topDownTW()
{

	//First, check if we found a bottom-up solution without running out of space
	if (bottomUpInfo.foundSolution())
	{
		std::cout << "Bottom up found solution\n";
		return bottomUpInfo.solution();
	}

	std::cout << "Bottom up finished after " << bottomUpInfo.levelReached() << " levels\n";

	VSet S(G);

	//Remove elements in the max-clique of G
	VSet maxClique = exactMaxClique(G);
	std::vector<Vertex> cliqueVec(maxClique.size());
	maxClique.members(cliqueVec);
	std::cout << "Max clique " << showSet(maxClique) << "\n";

	for (auto iter = cliqueVec.begin(); iter != cliqueVec.end(); ++iter)
	{
		S.erase(*iter);
	}

	sharedUpperBound = bottomUpInfo.bestUpperBound();


	lowerBound = d2degen(S, G);
	std::cout << "d2 lower " << lowerBound << "\n";
	lowerBound = std::max(lowerBound, MMD(S, G));
	std::cout << "MMD lower " << MMD(S, G) << "\n";

	std::cout << "Found lower bound " << lowerBound << "\n";


	return topDownTWFromSet(G, S, S.size());



}




int AbstractTopDown::topDownTWFromSet(const Graph& G, const VSet& S, int nSet)
{
	static int numCalled = 0;
	numCalled++;
	if (numCalled % 500000 == 0)
	{
		std::cout << "Called recursive fun " << numCalled << " times\n";
		std::cout << "Simplicial " << numSimplicial << " lower " << improvedByLocalLower << " pruned " << numPruned << "\n";
	}

	const int threshold = -1;



	if (S.empty())
	{
		return NO_WIDTH;
	}
	else if (nSet <= bottomUpInfo.levelReached())
	{
		auto searchInfo = bottomUpInfo.topLevelDict()->find(S);
		if (searchInfo == bottomUpInfo.topLevelDict()->end())
		{
			return sharedUpperBound;
		}
		else
		{
			return searchInfo->second;
		}
	}


	if (isMemoized(nSet, S)) //TODO replace with memo lookup
	{
		return memoizedValue(nSet, S);
	}
	else
	{
		std::vector<int> qValues(nGraph);
		findQvalues(nGraph, S, G, qValues);
		int minTW = sharedUpperBound;


		//Do a simplicial vertex first if it exists
		for (Vertex v : allVertices)
		{
			if (S.contains(v) && isSimplicial(G, v, S))
			{
				numSimplicial++;
				VSet SminusV(S);
				SminusV.erase(v);
				int subVal = topDownTWFromSet(G, SminusV, nSet - 1);
				int finalTW = minTW = std::min(minTW, std::max(qValues[v], subVal));
				this->memoizeTW(nSet, S, minTW);
				return finalTW;
			}
		}

		for (Vertex v : allVertices)
		{

			if (S.contains(v))
			{
				VSet SminusV(S);
				SminusV.erase(v);
				int q = qValues[v];


				//assert(setUpperBound >= sharedUpperBound);

				//Don't recursively calculate TW if we know Q is bigger

				/*
				if (q >= setUpperBound)
				{
				minTW = std::min(minTW, q);
				}*/

				if (q < minTW || q < sharedUpperBound)
				{
					int setUpperBound = sharedUpperBound;

					std::vector<Vertex> sortedMembers;
					SminusV.members(sortedMembers);

					setUpperBound = std::min(setUpperBound, permutTW(nGraph, S, sortedMembers, G));

					if (q >= setUpperBound || setUpperBound < lowerBound)
					{
						improvedByLocalLower++;
						minTW = std::min(minTW, q);
					}
					else
					{
						minTW = std::min(minTW, std::max(q, topDownTWFromSet(G, SminusV, nSet - 1)));
						sharedUpperBound = std::min(sharedUpperBound, std::max(minTW, nGraph - nSet - 1));
					}

				}
				else
				{
					numPruned++;
				}
			}

		}

		try
		{
			//Don't cache the top few layers, to save space
			//Same for the bottom few layers
			if (nGraph - nSet > topLevelNoStore && nSet > bottomLevelNoStore)
			{
				memoizeTW(nSet, S, minTW);
				if (numInDict % 50000 == 0)
				{
					std::cout << numInDict << " elements currently stored\n";
				}
			}
		}
		catch (const std::bad_alloc& e) {
			this->cleanMemoized();
			std::cout << numInDict << " elements stored in Dict\n";
			//std::cerr << "Emptying layer" << nSet << ", deleting" << TW[nSet].size() << " elements\n";
			//numInDict -= TW[nSet].size();
			//TW[nSet].clear(); //TODO smarter than emptying entire layer?

		}
		return minTW;
	}



}




