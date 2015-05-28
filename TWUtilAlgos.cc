#include "qset.hh"
#include <vector>

#include "TWUtilAlgos.hh"
#include "AbstractMemo.hh"

int permutTW(std::vector<Vertex> Svec, Graph G) {
    int tw = NO_WIDTH;
    
    while (!Svec.empty())
    {
	auto current = Svec.back();
	Svec.pop_back();
	VSet S(Svec.begin(), Svec.end());
	
	int qval = sizeQ(S, current, G);
	

        tw = std::max(tw, qval);
	
    }
    return tw;

    
}
