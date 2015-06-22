#include <vector>
#include <cstdlib>

#include "HeuristicUpperBoundMemo.hh"
#include "TWUtilAlgos.hh"

int HeuristicUpperBoundMemo::upperBound(VSet S)
{
    auto memb = S.members();
    
    auto Svec = S.members();
    
    Graph gthis = this->G;
    std::sort(Svec.begin(), Svec.end(), [this](Vertex u, Vertex v)
			    {
				auto ud = boost::degree(u, this->G);
				auto vd = boost::degree(v, this->G);
				return ud < vd;
			    } );

    return permutTW(Svec, G);
    
			    
}

int HeuristicUpperBoundMemo::lowerBound(VSet S)
{
    //The lower-bound for subgraph tree-width is also a lower-bound for our recurrence
    //TODO prove this
    return subgraphTWLowerBound(S, this->G);
}



HeuristicUpperBoundMemo::HeuristicUpperBoundMemo(Graph G) : BasicMemo(G)
{
}

HeuristicUpperBoundMemo::~HeuristicUpperBoundMemo()
{
}
