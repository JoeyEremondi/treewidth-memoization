#include "TopDownTW.hh"

#include "UpperBound.hh"
#include "BottomUpTW.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream
#include <cassert>

const int maxDictSize = 1000000000;

const int maxBottumUpSize = 1000000000;

const int topLevelNoStore = 15;

TopDownTW::TopDownTW(const Graph& gIn)
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

//TODO put this in its own file
bool isSimplicial(const Graph& G, Vertex v, const VSet& SStart)
{
	//std::vector<Vertex> neighbours;
	auto neighbPair = boost::adjacent_vertices(v, G);
	VSet neighbSet;

	//Create our neighbour set
	for (auto n = neighbPair.first; n != neighbPair.second; ++n)
	{
		if (SStart.contains(*n))
		{
			neighbSet.insert(*n);
		}
	}

	for (auto n = neighbPair.first; n != neighbPair.second; ++n)
	{
		auto nnSet = boost::adjacent_vertices(*n, G);
		for (auto nn = nnSet.first; nn != nnSet.second; ++nn)
		{
			if (SStart.contains(*nn) && !neighbSet.contains(*nn))
			{
				return false;
			}
		}
	}
	return true;
}

int TopDownTW::topDownTW(const Graph& G)
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

	for (auto iter = cliqueVec.begin(); iter != cliqueVec.end(); ++iter)
	{
		S.erase(*iter);
	}

	sharedUpperBound = bottomUpInfo.bestUpperBound();


	lowerBound = d2degen(S, G);
	lowerBound = std::max(lowerBound, MMD(S, G));

	std::cout << "Found lower bound " << lowerBound << "\n";


	return topDownTWFromSet(G, S, S.size());



}




int TopDownTW::topDownTWFromSet(const Graph& G, const VSet& S, int nSet)
{
	static std::vector<std::unordered_map<VSet, int>> TW(nGraph);

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

	auto setSearch = TW[nSet].find(S);
	auto setEnd = TW[nSet].end();

	if (setSearch != setEnd) //TODO replace with memo lookup
	{
		return setSearch->second;
	}
	/*
	else if (nSet < threshold)
	{
		return bottomUpTWFromSet(G, S, sharedUpperBound);
	}*/
	else
	{
		std::vector<int> qValues(nGraph);
		findQvalues(nGraph, S, G, qValues);
		int minTW = sharedUpperBound;


		//Do a simplicial vertex first if it exists
		for (Vertex v : allVertices)
		{
			if (false)//(S.contains(v) && isSimplicial(G, v, SStart))
			{
				VSet SminusV(S);
				SminusV.erase(v);
				int finalTW = minTW = std::min(minTW, std::max(qValues[v], topDownTWFromSet(G, SminusV, nSet - 1)));
				TW[nSet][S] = minTW;
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
				
				if (q < minTW || q < sharedUpperBound )
				{
					int setUpperBound = sharedUpperBound;

					std::vector<Vertex> sortedMembers;
					SminusV.members(sortedMembers);

					setUpperBound = permutTW(nGraph, S, sortedMembers, G);

					if (q >= setUpperBound || setUpperBound < lowerBound)
					{
						minTW = std::min(minTW, q);
					}
					else
					{
						minTW = std::min(minTW, std::max(q, topDownTWFromSet(G, SminusV, nSet - 1)));
						sharedUpperBound = std::min(sharedUpperBound, std::max(minTW, nGraph - nSet - 1));
					}

				}
			}

		}

		try
		{
			//Don't cache the top few layers, to save space
			//Same for the bottom few layers
			if (nGraph - nSet > topLevelNoStore && nSet > topLevelNoStore)
			{
				TW[nSet][S] = minTW;
				numInDict++;
				if (numInDict % 500000 == 0)
				{
					std::cerr << numInDict << " elements currently stored\n";
				}
			}
		}
		catch (const std::bad_alloc& e) {
			std::cerr << numInDict << " elements stored in Dict\n";
			std::cerr << "Emptying layer" <<  nSet << ", deleting" << TW[nSet].size() << " elements\n";
			numInDict -= TW[nSet].size();
			TW[nSet].clear(); //TODO smarter than emptying entire layer?
			
		}
		return minTW;
	}



}




