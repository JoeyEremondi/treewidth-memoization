#include <vector>
#include <set>
#include <cstdlib>

#include "HeuristicUpperBoundMemo.hh"
#include "TWUtilAlgos.hh"

int HeuristicUpperBoundMemo::upperBound(std::set<Vertex> S)
{
    std::vector<Vertex> Svec(S.begin(), S.end());
    Graph gthis = this->G;
    std::sort(Svec.begin(), Svec.end(), [this](Vertex u, Vertex v)
			    {
				auto ud = boost::degree(u, this->G);
				auto vd = boost::degree(v, this->G);
				return ud < vd;
			    } );

    return permutTW(Svec, G);
    
			    
}


HeuristicUpperBoundMemo::HeuristicUpperBoundMemo(Graph G) : BasicMemo(G)
{
}

HeuristicUpperBoundMemo::~HeuristicUpperBoundMemo()
{
}
