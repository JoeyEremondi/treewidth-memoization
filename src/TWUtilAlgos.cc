#include "qset.hh"
#include <vector>
#include <map>

#include "TWUtilAlgos.hh"

#include <boost/graph/subgraph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

#include <boost/graph/graphviz.hpp>

int permutTW(int nGraph, VSet vsetArg, const std::vector<Vertex>& Svec, const Graph& G) {
	int tw = NO_WIDTH;
	VSet vs = vsetArg;

	auto loopEnd = Svec.end();
	for (auto current = Svec.begin(); current != loopEnd; ++current)
	{
		Vertex v = *current;
		vs.erase(v);
		int qval = sizeQ(nGraph, vs, v, G);
		tw = std::max(tw, qval);
	}
	return tw;
}



std::pair<Graph, std::map<Vertex, Vertex>> complement_graph(const Graph& G)
{
	auto vertexInfo = boost::vertices(G);
	//Start GC as identical to G, gives us all the same vertices
	Graph GC;

	std::map<Vertex, Vertex> vmap;

	//Add all vertices from G to GC, and store them in a vertex map
	for (auto u = vertexInfo.first; u != vertexInfo.second; u++)
	{
		Vertex newVert = boost::add_vertex(GC);
		vmap[*u] = newVert;
	}


	//Add all the remaining edges to our compliment
	for (auto u = vertexInfo.first; u != vertexInfo.second; u++)
	{
		for (auto v = vertexInfo.first; v != vertexInfo.second; v++)
		{
			if (*u < *v && !(edge(*u, *v, G).second))
			{
				boost::add_edge(vmap[*u], vmap[*v], GC);
			}

		}

	}

	//Return our key-value set for the map
	//http://stackoverflow.com/questions/8321316/flip-map-key-value-pair
	std::map<Vertex, Vertex> reversed;

	for (auto i = vmap.begin(); i != vmap.end(); ++i)
		reversed[i->second] = i->first;

	std::pair<Graph, std::map<Vertex, Vertex>> ret(GC, reversed);
	return ret;

}


int subgraphDegree(Vertex v, VSet S, const Graph& G)
{
	VSet neighbours;

	auto neighbourIter = boost::adjacent_vertices(v, G);
	for (auto n = neighbourIter.first; n != neighbourIter.second; ++n)
	{
		neighbours.insert(*n);
	}

	return S.setIntersection(neighbours).size();

}

VSet maxIndSetHelper(VSet S, const Graph& G)
{
	//Base case
	if (S.size() <= 1)
	{
		return S;
	}

	auto minDegInfo = minDegreeSubgraphVert(S, G);

	Vertex minDegreeVert = minDegInfo.first;
	int minDeg = minDegInfo.second;


	//Try finding max IS with the min-degree vertex removed
	auto SV = S;
	//Erase each neighbour of the vertex, and the vertex itself
	//std::cout << "Size " << S.size() << " choosing " << minDegreeVert << "\n";
	SV.erase(minDegreeVert);
	auto svNeighbs = boost::adjacent_vertices(minDegreeVert, G);
	for (auto minNeighb = svNeighbs.first; minNeighb != svNeighbs.second; minNeighb++)
	{
		SV.erase(*minNeighb);
	}
	VSet bestFound = maxIndSetHelper(SV, G);
	bestFound.insert(minDegreeVert);

	//Then, iterate through each neighbour of V, and try having it in the IS
	auto neighbourSet = boost::adjacent_vertices(minDegreeVert, G);
	for (auto iter = neighbourSet.first; iter != neighbourSet.second; iter++)
	{
		//Ignore vertices not in the current subgraph
		if (S.contains(*iter))
		{
			auto SNew = S;
			//Erase each neighbour of this neighbour
			//And erase the neighbour itself
			SNew.erase(*iter);

			//Erase the min-degree vertex
			SNew.erase(minDegreeVert);

			auto nnSet = boost::adjacent_vertices(minDegreeVert, G);
			for (auto nneighb = nnSet.first; nneighb != nnSet.second; nneighb++)
			{
				SNew.erase(*nneighb);
			}

			VSet subFound = maxIndSetHelper(SNew, G);
			subFound.insert(*iter);
			if (bestFound.size() < subFound.size())
			{
				bestFound = subFound;
			}
		}


	}
	return bestFound;

}

VSet exactMaxIndSet(const Graph& G)
{
	VSet S(G);
	return maxIndSetHelper(S, G);

}


VSet exactMaxClique(const Graph& G)
{
	auto compInfo = complement_graph(G);

	VSet iset = exactMaxIndSet(compInfo.first);
	std::vector<Vertex> iVec;
	iset.members(iVec);

	//Translate the compliment vertices into our vertices
	VSet ret;

	for (auto iter = iVec.begin(); iter != iVec.end(); iter++)
	{
		ret.insert(compInfo.second[*iter]);
	}

	return ret;

}


//Approximate algorithms for clique and IS
//using the min-degree heuristic
//We can do this quickly using iteration
VSet approxIndSetHelper(VSet Sarg, const Graph& G)
{
	VSet S = Sarg;
	VSet returnSet;


	//Base case
	while (S.size() > 0)
	{
		std::vector<Vertex> members;
		S.members(members);
		Vertex minDegreeVert = members[0];
		int minDeg = subgraphDegree(members[0], S, G);

		for (auto iter = members.begin(); iter != members.end(); iter++)
		{
			int currentDeg = subgraphDegree(*iter, S, G);
			if (minDeg > currentDeg)
			{
				minDegreeVert = *iter;
				minDeg = currentDeg;
			}
		}

		//Assume the Max-IS contains the min-degree vertex
		//Erase each neighbour of the vertex, and the vertex itself
		auto svNeighbs = boost::adjacent_vertices(minDegreeVert, G);
		for (auto neighb = svNeighbs.first; neighb != svNeighbs.second; neighb++)
		{
			S.erase(*neighb);
		}
		S.erase(minDegreeVert);
		//The min-degree vertex is in our independent set, the neighbours are not
		returnSet.insert(minDegreeVert);
	}


	return returnSet;

}

VSet approxMaxIndSet(const Graph& G)
{
	VSet S(G);
	return approxIndSetHelper(S, G);

}

VSet approxMaxClique(const Graph& G)
{
	auto compInfo = complement_graph(G);
	VSet iset = approxMaxIndSet(compInfo.first);
	std::vector<Vertex> iVec;
	iset.members(iVec);

	//Translate the compliment vertices into our vertices
	VSet ret;

	for (auto iter = iVec.begin(); iter != iVec.end(); iter++)
	{
		ret.insert(compInfo.second[*iter]);
	}

	return ret;

}

int subgraphTWLowerBound(VSet S, const Graph& G, const Graph& GC)
{
	VSet aClique = approxIndSetHelper(S, GC);

	return aClique.size();
}

std::pair<Vertex, int> minDegreeSubgraphVert(VSet S, const Graph& G)
{
	std::vector<Vertex> members;
	S.members(members);

	Vertex minDegreeVert = members[0];
	int minDeg = subgraphDegree(members[0], S, G);

	for (auto iter = members.begin(); iter != members.end(); iter++)
	{
		int currentDeg = subgraphDegree(*iter, S, G);
		if (minDeg > currentDeg)
		{
			minDegreeVert = *iter;
			minDeg = currentDeg;
		}
	}

	return std::pair<Vertex, int>(minDegreeVert, minDeg);
}

//Fails if S doesn't have at least 2 vertices
std::pair<Vertex, int> minDegreeVertExcept(Vertex v, VSet S, const Graph& G)
{
	std::vector<Vertex> members;
	S.members(members);

	Vertex minDegreeVert;
	int minDeg;

	if (v == members[0])
	{
		minDegreeVert = members[1];
		minDeg = subgraphDegree(members[1], S, G);
	}
	else {
		minDegreeVert = members[0];
		minDeg = subgraphDegree(members[0], S, G);
	}

	for (auto iter = members.begin(); iter != members.end(); iter++)
	{
		int currentDeg = subgraphDegree(*iter, S, G);
		if (*iter != v && minDeg > currentDeg)
		{
			minDegreeVert = *iter;
			minDeg = currentDeg;
		}
	}

	return std::pair<Vertex, int>(minDegreeVert, minDeg);
}

//Based on algorithm 3 of Lower Bounds paper
int MMD(VSet S, const Graph& G)
{
	VSet H = S;
	int maxmin = 0;
	while (H.size() >= 2)
	{
		Vertex v = minDegreeSubgraphVert(H, G).first;
		maxmin = std::max(maxmin, subgraphDegree(v, H, G));
		H.erase(v);
	}

	return maxmin;

}

int minDeg(VSet S, const Graph& G)
{
	std::vector<Vertex> memb;
	S.members(memb);
	int minDeg = S.size();
	for (auto iter = memb.begin(); iter != memb.end(); iter++)
	{
		minDeg = std::min(minDeg, subgraphDegree(*iter, S, G));
	}


	return minDeg;

}

int secondSmallestDegree(VSet H, Graph G)
{
	std::vector<Vertex> members;
	H.members(members);
	std::sort(members.begin(), members.end(),
		[G, H](const Vertex & u, const Vertex & v) -> bool
	{
		return subgraphDegree(u, H, G) < subgraphDegree(v, H, G);
	});

	return subgraphDegree(members[1], H, G);
}

Vertex customMinDegreeExcept(Vertex vExclude, VSet H, Graph G)
{
	std::vector<Vertex> members;
	H.members(members);
	std::sort(members.begin(), members.end(),
		[G, H](const Vertex & u, const Vertex & v) -> bool
	{
		return subgraphDegree(u, H, G) < subgraphDegree(v, H, G);
	});
	
	if (members[0] == vExclude)
	{
		return members[1];
	}
	return members[0];
}



//Based on Algorithm 2 from the lower bounds paper
int d2degen(const VSet& SStart, const Graph& G)
{

	int d2D = 0;
	
	std::vector<Vertex> sMembers;
	SStart.members(sMembers);
	
	for (Vertex v : sMembers)
	{
		VSet H = SStart;
		
		while (H.size() >= 2)
		{
			d2D = std::max(d2D, secondSmallestDegree(H, G));
			Vertex u = customMinDegreeExcept(v, H, G);
			H.erase(u);
		}

	}
	return d2D;

}
