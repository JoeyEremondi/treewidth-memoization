#include <vector>
#include <cstdlib>

#include "SimplicialFirstMemo.hh"
#include "qset.hh"


int SimplicialFirstMemo::lowerBound(const VSet& S)
{
	//If the current set contains the special vertex, we didn't choose it as special
	//So we give it an impossibly large lower bound
	if (currentHasSimpOrSingle && S.contains(currentSpecialVertex))
	{
		return S.size() + 2;

	}
	//If no vertex is special, or we've removed the special one to recurse
	//We give the normal lower bound
	else {
		return BasicMemo::lowerBound(S);
	}


}

//Add our checks to the beginning
int SimplicialFirstMemo::subTW(int lowerBound, int upperBound, const VSet& S)
{
	std::vector<Vertex> members(S.size());
	S.members(members);
	for (auto iter = members.begin(); iter != members.end(); iter++)
	{
		if (isSimplicial(*iter, S) || hasOneNeighbour(*iter, S))
		{
			currentHasSimpOrSingle = true;
			//std::cout << "Setting special to " << *iter << "\n";
			currentSpecialVertex = *iter;
			//TODO how to choose if there are multiples?
		}
	}
	return BasicMemo::subTW(lowerBound, upperBound, S);


}

bool SimplicialFirstMemo::isSimplicial(Vertex v, const VSet& S)
{
	//TODO make faster?
	VSet adjInSet;
	auto adj = boost::adjacent_vertices(v, G);

	for (auto iter = adj.first; iter != adj.second; iter++)
	{
		if (S.contains(*iter))
		{
			adjInSet.insert(*iter);
		}

	}

	//Quadratically loop and ensure all the neighbours are connected
	//TODO make faster?
	std::vector<Vertex> adjMemb;
	adjInSet.members(adjMemb);
	for (auto iter = adjMemb.begin(); iter != adjMemb.end(); iter++)
	{
		for (auto iter2 = adjMemb.begin(); iter2 != adjMemb.end(); iter++)
		{
			if (!this->adjMatrix[*iter][*iter2])
			{
				return false;
			}

		}
	}

	return true;

}

bool SimplicialFirstMemo::hasOneNeighbour(Vertex v, const VSet& S)
{
	int numNeighbours = 0;
	auto adj = boost::adjacent_vertices(v, G);

	for (auto iter = adj.first; iter != adj.second; iter++)
	{
		if (S.contains(*iter))
		{
			numNeighbours++;
		}

	}

	//TODO implement
	return numNeighbours == 1;

}


SimplicialFirstMemo::SimplicialFirstMemo(Graph G) : BasicMemo(G)
{
}

SimplicialFirstMemo::~SimplicialFirstMemo()
{
}

