

#include <cstdlib>
#include <iostream>
#include <climits>
#include <algorithm>
#include <vector>
#include <utility>
#include <ctime>

#include "qset.hh"
#include "graphTypes.hh"

int numQCalled = 0;


//Basically just a DFS
//We start at v, and search
//We expand vertices in S
//And we add to our count (without expanding) for those not in S
int sizeQ(int n, const VSet &S, Vertex v, const Graph& G)
{
	numQCalled++;

	//std::cout << "Q: starting with S = " << showSet(S) << "\n";

	//int n = boost::num_vertices(G);

	Vertex open[MAX_NUM_VERTICES];

	open[0] = v;
	int stackEnd = 0;

	//Uses more memory than VSet, but is faster
	bool closed[MAX_NUM_VERTICES] = { false };

	int numInQ = 0;



	while (stackEnd >= 0)
	{
		Vertex w = open[stackEnd];
		stackEnd--;
		auto& outEdges = boost::out_edges(w, G);
		//std::cout << "Q: expanding " << w << "\n";

		for (auto iter = outEdges.first; iter != outEdges.second; iter++)
		{
			Edge e = *iter;
			Vertex u = boost::target(e, G);
			//std::cout << "Q: found neighbour " << u << "\n";
			if (!closed[u])
			{
				//std::cout << "Q: adding " << u << " to closed\n";

				closed[u] = true;

				if (u != v && S.contains(u))
				{
					//std::cout << "Q: adding " << u << " to queue\n";
					stackEnd++;
					open[stackEnd] = u;
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
	numQCalled++;

	//std::cout << "Multi Q: starting with S = " << showSet(S) << "\n";

	//int n = boost::num_vertices(G);

	//Store which vertices we've seen over all DFSes, makes sure we get all connected components
	VSet globalUnseen = S;

	//Store the set of vertices each connected component can reach
	std::vector<VSet> canReach(n);

	//For each vertex not in S, store which connected components it connects to
	std::vector<std::vector<Vertex>> reachableFrom(n);

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


				auto& outEdges = boost::out_edges(w, G);
				//std::cout << "Q: expanding " << w << "\n";

				for (auto iter = outEdges.first; iter != outEdges.second; iter++)
				{
					Edge e = *iter;
					Vertex u = boost::target(e, G);


					//std::cout << "Q: found neighbour " << u << "\n";
					if (!closed.contains(u))
					{
						//std::cout << "Q: adding " << u << " to closed\n";

						closed.insert(u);

						if (S.contains(u))
						{
							//std::cout << "Q: adding " << u << " to queue\n";
							open.push_back(u);


						}
						else
						{
							//Mark that this CC and our vertex u, not in s, can reach each other
							reachableFrom[u].push_back(currentCC);
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
	for (auto iter = vertInfo.first; iter != vertInfo.second; iter++)
	{
		Vertex v = *iter;
		VSet allReachableVerts;
		for (auto connComp = reachableFrom[v].begin(); connComp != reachableFrom[v].end(); connComp++)
		{
			allReachableVerts.addAll(canReach[*connComp]);
		}

		auto& outEdges = boost::out_edges(v, G);
		//std::cout << "Q: expanding " << w << "\n";

		for (auto iter = outEdges.first; iter != outEdges.second; iter++)
		{
			Edge e = *iter;
			Vertex u = boost::target(e, G);
			if (!S.contains(u))
			{
				allReachableVerts.insert(u);
;			}
		}

		//std::cout << "VSet for " << v << " size " << allReachableVerts.size() << " set " << showSet(allReachableVerts) << "\n";

		allReachableVerts.erase(v);
		outValues[v] = allReachableVerts.size();
	}
	

}



std::string showSet(VSet S) {
	std::ostringstream result;

	result << "{";

	std::vector<Vertex> members;
	S.members(members);

	for (auto iter = members.begin(); iter != members.end(); iter++)
	{
		result << *iter << " ; ";
	}

	result << "}";

	return result.str();

}
