#include <vector>
#include <cstdlib>

#include "HeuristicUpperBoundMemo.hh"
#include "TWUtilAlgos.hh"

int HeuristicUpperBoundMemo::upperBound(VSet S)
{
	/*
	if (S.asInt() % 13 == 0)
	{

	auto Svec = S.members();

	Graph gthis = this->G;
	//TODO ascending or descending?
	std::sort(Svec.begin(), Svec.end(), [this](Vertex u, Vertex v)
				{
				auto ud = boost::degree(u, this->G);
				auto vd = boost::degree(v, this->G);
				return ud > vd;
				} );

	return permutTW(S, Svec, G);
	}*/
	std::vector<Vertex> members;
	S.members(members);
	return permutTW(S, members, G);
	//return BasicMemo::upperBound(S);



}

int HeuristicUpperBoundMemo::lowerBound(VSet S)
{
	//The lower-bound for subgraph tree-width is also a lower-bound for our recurrence
	//TODO prove this
	//if (S.asInt() % 13 == 0)
	//{
	return subgraphTWLowerBound(S, this->G, this->GC);
	//}
	//return MMD(S, G); //0;
	//return 0;
}



HeuristicUpperBoundMemo::HeuristicUpperBoundMemo(Graph G) : BasicMemo(G)
{
}

HeuristicUpperBoundMemo::~HeuristicUpperBoundMemo()
{
}
