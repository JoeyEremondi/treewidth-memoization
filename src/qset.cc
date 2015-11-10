

#include <cstdlib>
#include <iostream>
#include <climits>
#include <algorithm>
#include <vector>
#include <utility>
#include <ctime>

#include "qset.hh"
#include "graphTypes.hh"

#include <boost/graph/visitors.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/subgraph.hpp>


//Basically just a DFS
//We start at v, and search
//We expand vertices in S
//And we add to our count (without expanding) for those not in S
int sizeQ(int n, const VSet &S, Vertex v, const Graph& G)
{

	std::vector<Vertex> open;
	open.reserve(n);
	open.push_back(v);

	VSet closed;

	int numInQ = 0;



	while (!open.empty())
	{
		Vertex w = open.back();
		open.pop_back();

		auto outEdges = boost::out_edges(w, G);

		auto loopEnd = outEdges.second;
		for (auto iter = outEdges.first; iter != loopEnd; ++iter)
		{
			Edge e = *iter;
			Vertex u = boost::target(e, G);

			if (!closed.contains(u))
			{
				closed.insert(u);

				if (S.contains(u) && u != v)
				{
					open.push_back(u);
				}
				else if (u != v)
				{
					numInQ++;

				}
			}
		}
	}

	return numInQ;

}

void findQvalues(int n, const VSet &S, const Graph &G, std::vector<int>& outValues)
{

	//Store which vertices we've seen over all DFSes, makes sure we get all connected components
	VSet globalUnseen = S;

	//Store the set of vertices each connected component can reach
	std::vector<VSet> canReach(n);


	//For each vertex not in S, store which connected components it connects to
	std::vector<std::vector<Vertex>> reachableFrom(n);
	for (auto& subVec : reachableFrom)
	{
		subVec.reserve(n);
	}

	//Also store it as a set, to keep the vector as small as possible
	std::vector<VSet> reachableFromSet(n);

	if (!S.empty())
	{
		while (!globalUnseen.empty())
		{
			std::vector<Vertex> open;

			Vertex first = globalUnseen.first();
			Vertex currentCC = first;

			open.push_back(first);


			//Vertices we've already seen
			VSet closed;


			while (!open.empty())
			{
				Vertex w = open.back();
				open.pop_back();

				//Mark that we've seen this vertex in our connected component search
				globalUnseen.erase(w);


				auto outEdges = boost::out_edges(w, G);

				for (auto iter = outEdges.first; iter != outEdges.second; ++iter)
				{
					Vertex u = boost::target(*iter, G);


					if (!closed.contains(u))
					{

						closed.insert(u);

						if (S.contains(u))
						{
							open.push_back(u);
							reachableFrom[u].push_back(currentCC);

						}
						else if (!reachableFromSet[u].contains(currentCC))
						{
							//Mark that this CC and our vertex u, not in s, can reach each other
							reachableFrom[u].push_back(currentCC);
							reachableFromSet[u].insert(currentCC);
							canReach[currentCC].insert(u);
						}
					}
				}
			}
		}
	}

	//For each vertex, we union the reachable vertices from each connected component
	//and take the number of elements
	//We also add in any vertices immediately adjacent to v which aren't in S
	auto vertInfo = vertices(G);
	for (auto iter = vertInfo.first; iter != vertInfo.second; ++iter)
	{
		Vertex v = *iter;
		VSet allReachableVerts;
		auto loopEnd = reachableFrom[v].end();


		for (auto connComp = reachableFrom[v].begin(); connComp != loopEnd; ++connComp)
		{
			allReachableVerts.addAll(canReach[*connComp]);
		}

		auto outEdges = boost::out_edges(v, G);

		//Every edge of our current vertex is also in its Q set, unless it's in S
		for (auto iter = outEdges.first; iter != outEdges.second; ++iter)
		{
			Vertex u = boost::target(*iter, G);
			if (!S.contains(u))
			{
				allReachableVerts.insert(u);
				;
			}
		}

		allReachableVerts.erase(v);
		outValues[v] = allReachableVerts.size();
	}


}





