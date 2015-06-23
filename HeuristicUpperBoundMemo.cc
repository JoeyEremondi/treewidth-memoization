#include <vector>
#include <cstdlib>

#include "HeuristicUpperBoundMemo.hh"
#include "TWUtilAlgos.hh"

int HeuristicUpperBoundMemo::upperBound(VSet S)
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
    
			    
}

int HeuristicUpperBoundMemo::lowerBound(VSet S)
{
    //The lower-bound for subgraph tree-width is also a lower-bound for our recurrence
    //TODO prove this
    return subgraphTWLowerBound(S, this->G, this->GC);
}



HeuristicUpperBoundMemo::HeuristicUpperBoundMemo(Graph G) : BasicMemo(G)
{
}

HeuristicUpperBoundMemo::~HeuristicUpperBoundMemo()
{
}
