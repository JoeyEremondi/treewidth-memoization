#include <vector>
#include <cstdlib>

#include "BasicMemo.hh"

//Ordered vertices: basic version doesn't order anything
void BasicMemo::orderVertices(std::vector<Vertex>& vec)
{

}

//Basic version: upper bound is the size of the set
int BasicMemo::upperBound(const VSet& S)
{
	return numVerts;
}

//Basic version: lower bound is always 1, nothing fancy
int BasicMemo::lowerBound(const VSet& S)
{
	return 1;
}

//Always cache for basic memoization
bool BasicMemo::shouldCache(const VSet& S)
{
	return true;
}

//Never clean in the basic version
bool BasicMemo::needsCleaning()
{
	return false;
}

//Should never get called
std::vector<VSet> BasicMemo::setsToForget(const VSet& S)
{
	return std::vector<VSet>();
}

BasicMemo::BasicMemo(Graph G) : AbstractMemo(G)
{
}

BasicMemo::~BasicMemo()
{
}



