#include "Simplicial.hh"


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