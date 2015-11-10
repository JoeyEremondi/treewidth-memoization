#include "Simplicial.hh"


bool isSimplicial(const Graph& G, Vertex v, const VSet& SStart)
{
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

	//For each neighbour pair, check if the edge is there
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