#include <vector>
#include <cstdlib>

#include "NaiveMemo.hh"

//Ordered vertices: basic version doesn't order anything
std::vector<Vertex> NaiveMemo::orderVertices(std::set<Vertex> S)
{
	std::vector<Vertex> v(S.begin(), S.end());
	return v;
}

//Basic version: upper bound is the size of the set
int NaiveMemo::upperBound(std::set<Vertex> S)
{
	return S.size();
}

//Basic version: lower bound is always 1, nothing fance
int NaiveMemo::lowerBound(std::set<Vertex> S)
{
	return 1;
}

//Never cache in naive version
bool NaiveMemo::shouldCache(std::set<Vertex> S)
{
	return false;
}

//Never clean in the basic version
bool NaiveMemo::needsCleaning()
{
	return false;
}

//Should never get called
std::vector<std::set<Vertex>> NaiveMemo::setsToForget(std::set<Vertex> S)
{
	return std::vector<std::set<Vertex>>();
}

NaiveMemo::NaiveMemo(Graph G) : AbstractMemo(G)
{
}

NaiveMemo::~NaiveMemo()
{
}


