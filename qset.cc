

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
	//std::cout << "Q: starting with S = " << showSet(S) << "\n";

	//int n = boost::num_vertices(G);

	std::vector<Vertex> open; //TODO replace
	open.reserve(n);
	open.push_back(v);

	//Uses more memory than VSet, but is faster
	VSet closed; //TODO replace

	int numInQ = 0;



	while (!open.empty())
	{
		Vertex w = open.back();
		open.pop_back();

		auto outEdges = boost::out_edges(w, G);
		//std::cout << "Q: expanding " << w << "\n";

		for (auto iter = outEdges.first; iter != outEdges.second; ++iter)
		{
			Edge e = *iter;
			Vertex u = boost::target(e, G);
			//std::cout << "Q: found neighbour " << u << "\n";
			if (!closed.contains(u))
			{
				//std::cout << "Q: adding " << u << " to closed\n";

				closed.insert(u);

				if (u != v && S.contains(u))
				{
					//std::cout << "Q: adding " << u << " to queue\n";
					open.push_back(u);
				}
				else if (u != v)
				{
					numInQ++;

				}
			}
		}
	}
	//delete[] closed;
	// delete[] open;
	//std::cout << "|Q| = " << numInQ << "\n";
	return numInQ;

}

void findQvalues(int n, const VSet &S, const Graph &G, std::vector<int>& outValues)
{
	//std::cout << "Multi Q: starting with S = " << showSet(S) << "\n";

	//int n = boost::num_vertices(G);

	//Store which vertices we've seen over all DFSes, makes sure we get all connected components
	VSet globalUnseen = S;

	//Store the set of vertices each connected component can reach
	std::vector<VSet> canReach(n);


	//For each vertex not in S, store which connected components it connects to
	std::vector<std::vector<Vertex>> reachableFrom(n);
	for (int i = 0; i < n; i++)
	{
		reachableFrom[i].reserve(n);
	}

	//Also store it as a set, to keep the vector as small as possible
	std::vector<VSet> reachableFromSet(n);

	//If S is empty: then we just see how many vertices 
	if (S.empty())
	{

	}
	else
	{
		while (!globalUnseen.empty())
		{
			std::vector<Vertex> open;

			Vertex first = globalUnseen.first();
			Vertex currentCC = first;

			//std::cout << "CC " << currentCC << "\n";

			open.push_back(first);


			//Vertices we've already seen
			VSet closed;


			while (open.size() > 0)
			{
				Vertex w = open.back();
				open.pop_back();

				//Mark that we've seen this vertex in our connected component search
				globalUnseen.erase(w);


				auto outEdges = boost::out_edges(w, G);
				//std::cout << "Q: expanding " << w << "\n";

				for (auto iter = outEdges.first; iter != outEdges.second; ++iter)
				{
					Vertex u = boost::target(*iter, G);


					//std::cout << "Q: found neighbour " << u << "\n";
					if (!closed.contains(u))
					{
						//std::cout << "Q: adding " << u << " to closed\n";

						closed.insert(u);

						if (S.contains(u))
						{
							//std::cout << "Q: adding " << u << " to queue\n";
							open.push_back(u);
							reachableFrom[u].push_back(currentCC);

						}
						else if (!reachableFromSet[u].contains(currentCC))
						{
							//Mark that this CC and our vertex u, not in s, can reach each other
							reachableFrom[u].push_back(currentCC);
							reachableFromSet[u].insert(currentCC);
							canReach[currentCC].insert(u);
							//std::cout << "Reached " << u << " from " << w << " in CC " << currentCC << "\n";
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
		//std::cout << "Q: expanding " << w << "\n";

		//Every edge of our current vertex is also in its Q set, unless it's in S
		for (auto iter = outEdges.first; iter != outEdges.second; ++iter)
		{
			//Edge e = *iter;
			Vertex u = boost::target(*iter, G);
			if (!S.contains(u))
			{
				allReachableVerts.insert(u);
;			}
		}

		allReachableVerts.erase(v);
		outValues[v] = allReachableVerts.size();
	}
	

}

int qCheck(int n, VSet S, Vertex v, Graph G)
{
	int numInQ = 0;
	auto vertInfo = vertices(G);
	auto edgeInfo = edges(G);
	for (auto iter = vertInfo.first; iter != vertInfo.second; iter++)
	{
		Vertex u = *iter;
		if (u != v && !S.contains(u))
		{
			VSet inducingSet = S;
			inducingSet.insert(v);
			inducingSet.insert(u);
			std::vector<Vertex> members;
			inducingSet.members(members);

			Graph GG;
			for (auto addVert = vertInfo.first; addVert != vertInfo.second; addVert++)
			{
					boost::add_vertex(GG);
			}

			for (auto edgeIter = edgeInfo.first; edgeIter != edgeInfo.second; edgeIter++)
			{
				Vertex from = boost::source(*edgeIter, G); 
				Vertex to = boost::target(*edgeIter, G);
				if (inducingSet.contains(to) && inducingSet.contains(from))
				{
					boost::add_edge(to, from, GG);
				}
			}

			std::vector<int> distances(boost::num_vertices(G) + 1);
			boost::breadth_first_search(GG,
				v,
				boost::visitor(boost::make_bfs_visitor(boost::record_distances(&distances[0], boost::on_tree_edge()))));
			if (distances[u] != 0)
			{
				numInQ++;
			}

		}
		
	}
	return numInQ;
}



std::string showSet(VSet S) {
	std::ostringstream result;

	result << "{";

	std::vector<Vertex> members;
	S.members(members);

	for (auto iter = members.begin(); iter != members.end(); ++iter)
	{
		result << *iter << " ; ";
	}

	result << "}";

	return result.str();

}
