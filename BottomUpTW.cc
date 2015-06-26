#include "BottomUpTW.hh"

#include <algorithm>

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
    
    return bottomUpTWFromSet(G, S, upperBound);


    
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
			    } );
    globalUpperBound = std::min(globalUpperBound, permutTW(S, Svec, G));

    //Third try: order by degree descending
    //Svec = S.members();
    std::sort(Svec.begin(), Svec.end(), [G](Vertex u, Vertex v)
			    {
				auto ud = boost::degree(u, G);
				auto vd = boost::degree(v, G);
				return ud > vd;
			    } );
    globalUpperBound = std::min(globalUpperBound, permutTW(S, Svec, G)); 

    std::cout << "Found global upper bound " << globalUpperBound << "\n"; 

    return globalUpperBound;
    
}

int bottomUpTWFromSet(const Graph& G, const VSet& SStart, int upperBound)
{
    std::unordered_map<VSet, int> TW[MAX_NUM_VERTICES];
    VSet emptySet;
    TW[0][emptySet] = NO_WIDTH;

    int n = boost::num_vertices(G);
    
    auto vertInfo = boost::vertices(G);

    for (int i = 1; i < SStart.size(); i++)
    {
	std::cout << "Level " << i << "\n";
	
	for (auto pair = TW[i-1].begin(); pair != TW[i-1].end(); pair++)
	{
	    VSet S = (*pair).first;
	    int r = (*pair).second;
	    
	    for (auto x = vertInfo.first; x != vertInfo.second; x++)
	    {
		if (!S.contains(*x))
		{
		    
		    VSet SUx = S;
		    SUx.insert(*x);
		
		    int q = sizeQ(n, S, *x, G);
		    int rr = std::min(r,q);
		    if (rr < upperBound)
		    {
			auto searchInfo = TW[i].find(SUx);
			if (searchInfo == TW[i].end())
			{
			    TW[i][SUx] = rr;
			} else {
			    TW[i][SUx] = std::min(searchInfo->second, rr);
			}
		    
		    
		    }
		}
		
	    }
	    
	}
	
    }

    auto searchInfo = TW[SStart.size()].find(SStart);
    if (searchInfo == TW[SStart.size()].end())
    {
	return upperBound;
    }
    return searchInfo->second;
    
    
    
}




