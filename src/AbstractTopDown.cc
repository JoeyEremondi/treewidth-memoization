#include "AbstractTopDown.hh"

#include "UpperBound.hh"
#include "BottomUpTW.hh"

#include "Simplicial.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream
#include <cassert>



AbstractTopDown::AbstractTopDown(const Graph& gIn, int maxBottom)
	: G(gIn)
	, maxBottomUpSize(maxBottom)
	, allVertices(boost::vertices(G).first, boost::vertices(G).second)
	, bottomUpInfo(G, maxBottomUpSize)
{
	nGraph = boost::num_vertices(G);
	//As a heuristic, we sort our vertices by degree
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

	//Take the upper bound that the bottom-up method found
	sharedUpperBound = bottomUpInfo.bestUpperBound();

	//Try setting our lower-bound to the higest value we can find
	lowerBound = d2degen(S, G);
	lowerBound = std::max(lowerBound, MMD(S, G));

	std::cout << "Found lower bound " << lowerBound << "\n";


	return topDownTWFromSet(G, S, S.size());



}




int AbstractTopDown::topDownTWFromSet(const Graph& G, const VSet& S, int nSet)
{

	//Base case: no width for empty set, -infinity
	if (S.empty())
	{
		return NO_WIDTH;
	}
	else if (nSet <= bottomUpInfo.levelReached())
	{
		//If we've recursed deep enough to see our bottom-up values, take from there
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

	//Don't calculate if we've already stored this value
	if (isMemoized(nSet, S))
	{
		return memoizedValue(nSet, S);
	}
	else
	{
		std::vector<int> qValues(nGraph);
		findQvalues(nGraph, S, G, qValues);
		int minTW = sharedUpperBound;


		//Optimization: Do a simplicial vertex first if it exists
		for (Vertex v : allVertices)
		{
			if (S.contains(v) && isSimplicial(G, v, S))
			{
				VSet SminusV(S);
				SminusV.erase(v);
				int subVal = topDownTWFromSet(G, SminusV, nSet - 1);
				int finalTW = minTW = std::min(minTW, std::max(qValues[v], subVal));
				this->memoizeTW(nSet, S, minTW);
				return finalTW;
			}
		}

		//Try our recurrence relation for each vertex in our set S
		for (Vertex v : allVertices)
		{

			if (S.contains(v))
			{
				VSet SminusV(S);
				SminusV.erase(v);
				int q = qValues[v];

				//We only need to try this value if it doesn't exceed our upper bound
				if (q < minTW || q < sharedUpperBound)
				{
					int setUpperBound = sharedUpperBound;

					std::vector<Vertex> sortedMembers;
					SminusV.members(sortedMembers);

					setUpperBound = std::min(setUpperBound, permutTW(nGraph, S, sortedMembers, G));

					//Don't compute a recursive value if we know that it violates our
					//Upper and lower bound constraints
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
			memoizeTW(nSet, S, minTW);
		}
		catch (const std::bad_alloc& e) {
			//If we run out of memory, run the cleaning procedure
			this->cleanMemoized();
		}
		return minTW;
	}


}

int AbstractTopDown::getMaxBottomUp()
{
	return maxBottomUpSize;
}




