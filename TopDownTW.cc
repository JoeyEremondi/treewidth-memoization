#include "TopDownTW.hh"

#include "UpperBound.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream
#include <cassert>


int topDownTW(const Graph& G)
{
	VSet S(G);

	//Remove elements in the max-clique of G
	VSet maxClique = exactMaxClique(G);
	std::vector<Vertex> cliqueVec(maxClique.size());
	maxClique.members(cliqueVec);

	for (auto iter = cliqueVec.begin(); iter != cliqueVec.end(); ++iter)
	{
		S.erase(*iter);
	}

	int upperBound = calcUpperBound(G, S);

	return topDownTWFromSet(G, S, upperBound);



}




int topDownTWFromSet(const Graph& G, const VSet& SStart, int globalUpperBound)
{
	static std::vector<std::unordered_map<VSet, int>> TW(SStart.size()+1);

	int nSet = SStart.size();
	int nGraph = boost::num_vertices(G);



	if (SStart.empty())
	{
		return NO_WIDTH;
	}

	auto setSearch = TW[nSet].find(SStart);
	auto setEnd = TW[nSet].end();

	if (setSearch != setEnd) //TODO replace with memo lookup
	{
		return setSearch->second;
	}
	else
	{
		std::vector<int> qValues(nGraph);
		findQvalues(boost::num_vertices(G), SStart, G, qValues);
		int minTW = globalUpperBound;

		std::vector<Vertex> members;
		SStart.members(members);

		for (Vertex v : members)
		{
			VSet SminusV(SStart);
			SminusV.erase(v);
			int q = qValues[v];
			//int qTrue = qCheck(nGraph, SminusV, v, G);
			//std::cerr << "set: " << showSet(SminusV) << "\n";
			//std::cerr << "v " << v << " Q " << q << " q true " << qTrue << "\n";
			//assert(q == qTrue);
			if (q < minTW && q < globalUpperBound)
			{
				minTW = std::min(minTW, std::max(q, topDownTWFromSet(G, SminusV, globalUpperBound)));
			}
			
		}

		TW[nSet][SStart] = minTW;
		return minTW;
	}



}




