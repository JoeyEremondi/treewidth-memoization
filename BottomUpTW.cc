#include "BottomUpTW.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream

int bottomUpTW(Graph G)
{
	VSet S(G);

	//Remove elements in the max-clique of G
	VSet maxClique = exactMaxClique(G);
	std::vector<Vertex> cliqueVec(maxClique.size());
	maxClique.members(cliqueVec);

	for (auto iter = cliqueVec.begin(); iter != cliqueVec.end(); iter++)
	{
		S.erase(*iter);
	}

	int upperBound = calcUpperBound(G, S);

	return bottomUpTWFromSet(maxClique, G, S, upperBound);



}

int calcUpperBound(Graph G, VSet S)
{
	//Optimiation: set the golbal upper-bound to the TW from some linear ordering
	//First try: default ordering
	std::vector<Vertex> Svec(S.size());
	S.members(Svec);

	int globalUpperBound = permutTW(S, Svec, G);

	//Second try: order by degree ascending
	std::sort(Svec.begin(), Svec.end(), [G](Vertex u, Vertex v)
	{
		auto ud = boost::degree(u, G);
		auto vd = boost::degree(v, G);
		return ud < vd;
	});
	globalUpperBound = std::min(globalUpperBound, permutTW(S, Svec, G));

	//Third try: order by degree descending
	//Svec = S.members();
	std::sort(Svec.begin(), Svec.end(), [G](Vertex u, Vertex v)
	{
		auto ud = boost::degree(u, G);
		auto vd = boost::degree(v, G);
		return ud > vd;
	});
	globalUpperBound = std::min(globalUpperBound, permutTW(S, Svec, G));

	std::cout << "Found global upper bound " << globalUpperBound << "\n";

	return globalUpperBound;

}

std::string showLayer(std::unordered_map<VSet, int> TW)
{
	std::ostringstream out;
	for (auto iter = TW.begin(); iter != TW.end(); iter++)
	{
		out << "{" << showSet(iter->first) << "=" << iter->second << "}, ";
	}
	return out.str();
}

/*
#if defined (__GNUC__) && ( __GNUC__ >= 3 )
typedef __gnu_cxx::hash_map< HashKey,
#else
typedef std::hash_map< HashKey,
#endif
unsigned,
hash_compare_for_pairs,
hash_equalkey_for_pairs > VertexSetHashMap;
*/

int maybeMin(int x, int y)
{
	if (x == -1)
	{
		return y;
	}
	if (y == -1)
	{
		return x;
	}
	return std::min(x, y);
}

int bottomUpTWFromSet(VSet clique, const Graph& G, const VSet& SStart, int globalUpperBound)
{
	std::unordered_map<VSet, int> TW[MAX_NUM_VERTICES];
	VSet emptySet;
	TW[0][emptySet] = NO_WIDTH;

	//TODO remove
	globalUpperBound = 26;

	int n = SStart.size();// boost::num_vertices(G);
	int nGraph = boost::num_vertices(G);

	std::vector<Vertex> vertInfo;
	SStart.members(vertInfo); // boost::vertices(G);

	int upperBound = globalUpperBound;

	for (int i = 1; i < SStart.size(); i++)
	{
		std::cout << "Level " << i << "\n";
		//std::cout << "Num Q " << numQCalled << "\n";

		//TODO what is this? Taken from Java version
		int minTW = upperBound;

		for (auto pair = TW[i - 1].begin(); pair != TW[i - 1].end(); pair++)
		{
			VSet S = (*pair).first;
			int r = (*pair).second;

			Vertex firstSet = S.first();

			for (auto x = vertInfo.begin(); x != vertInfo.end(); x++)
			{
				if (!S.contains(*x) && !clique.contains(*x) && (*x) < firstSet ) //TODO check if in clique here?
				{

					

					VSet SUx = S;
					SUx.insert(*x);

					int q = sizeQ(n, S, *x, G);
					//int rr = maybeMin(r, q);
					int rr = std::max(r, q);

					if (rr >= nGraph - i - 1 && rr < minTW) {
						minTW = rr;
					}

					if (rr < upperBound)
					{
						auto searchInfo = TW[i].find(SUx);
						if (searchInfo == TW[i].end())
						{
							TW[i][SUx] = rr;
						}
						else {
							TW[i][SUx] = std::min(searchInfo->second, rr);
						}


					}
				}

			}

		}

		//TODO right place in loop?
		upperBound = std::min(upperBound, std::max(minTW, nGraph - i - 1));

		std::cout << "TW i size: " << i << " " << TW[i].size() << "\n";
		if (i < 3)
		{
			
			//std::cout << "TW i: " << i << " " << showLayer(TW[i]) << "\n";
		}
		//std::cout << "TW i: " << i << " " << TW[i].size() << "\n";
	}

	auto searchInfo = TW[SStart.size()].find(SStart);
	if (searchInfo == TW[SStart.size()].end())
	{
		return upperBound;
	}
	return searchInfo->second;



}



