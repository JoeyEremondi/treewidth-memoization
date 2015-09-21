#include "TopDownTW.hh"

#include "UpperBound.hh"
#include "BottomUpTW.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream
#include <cassert>

TopDownTW::TopDownTW(const Graph& gIn)
	: G(gIn)
	, allVertices(boost::vertices(G).first, boost::vertices(G).second)
	, bottomUpInfo(G, 10000000)
{
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
		return bottomUpInfo.solution();
	}

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
	

	return topDownTWFromSet(G, S, S.size());



}




int TopDownTW::topDownTWFromSet(const Graph& G, const VSet& S, int nSet)
{
	static std::vector<std::unordered_map<VSet, int>> TW(S.size() + 1);

	int nGraph = boost::num_vertices(G);
	const int threshold = -1;



	if (S.empty())
	{
		return NO_WIDTH;
	}
	else if (nSet <= bottomUpInfo.levelReached())
	{
		return (*bottomUpInfo.topLevelDict())[S];
	}

	auto setSearch = TW[nSet].find(S);
	auto setEnd = TW[nSet].end();

	if (setSearch != setEnd) //TODO replace with memo lookup
	{
		return setSearch->second;
	}
	else if (nSet < threshold)
	{
		return bottomUpTWFromSet(G, S, sharedUpperBound);
	}
	else
	{
		std::vector<int> qValues(nGraph);
		findQvalues(boost::num_vertices(G), S, G, qValues);
		int minTW = sharedUpperBound;


		//Do a simplicial vertex first if it exists
		for (Vertex v : allVertices)
		{
			if (false)//(S.contains(v) && isSimplicial(G, v, SStart))
			{
				VSet SminusV(S);
				SminusV.erase(v);
				int finalTW = minTW = std::min(minTW, std::max(qValues[v], topDownTWFromSet(G, SminusV, nSet-1)));
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

				if (q < std::min(minTW, sharedUpperBound))
				{
					minTW = std::min(minTW, std::max(q, topDownTWFromSet(G, SminusV, nSet-1)));
					sharedUpperBound = std::min(sharedUpperBound, std::max(minTW, nGraph - nSet - 1));
				}
			}

		}

		TW[nSet][S] = minTW;
		return minTW;
	}



}




