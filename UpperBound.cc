#include "UpperBound.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream
#include <cassert>

int calcUpperBound(const Graph& G, const VSet& S)
{
	int nGraph = boost::num_vertices(G);

	//Optimiation: set the golbal upper-bound to the TW from some linear ordering
	//First try: default ordering
	std::vector<Vertex> Svec(S.size());
	S.members(Svec);

	int globalUpperBound = permutTW(nGraph, S, Svec, G);


	//Second try: order by degree ascending
	std::sort(Svec.begin(), Svec.end(), [G](Vertex u, Vertex v)
	{
		auto ud = boost::degree(u, G);
		auto vd = boost::degree(v, G);
		return ud < vd;
	});
	globalUpperBound = std::min(globalUpperBound, permutTW(nGraph, S, Svec, G));

	//Third try: order by degree descending
	//Svec = S.members();
	std::sort(Svec.begin(), Svec.end(), [G](Vertex u, Vertex v)
	{
		auto ud = boost::degree(u, G);
		auto vd = boost::degree(v, G);
		return ud > vd;
	});
	globalUpperBound = std::min(globalUpperBound, permutTW(nGraph, S, Svec, G));

	std::cout << "Found global upper bound " << globalUpperBound << "\n";

	return globalUpperBound;

}