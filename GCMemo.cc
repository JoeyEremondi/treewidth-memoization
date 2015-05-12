#include "GCMemo.hh"
#include "BasicMemo.hh"

GCMemo::GCMemo (unsigned long bound, unsigned long maxElemsParam) : DepthBoundedMemo(bound)
{
    maxElems = maxElemsParam;
}

int GCMemo::subTW(std::set<Vertex> S, Graph G)
{
    recordMemoInfo(S);
    
    if (S.size() >= maxElems)
    {
	auto toRemove = elemsToRemove();
	for (auto iter = toRemove.begin(); iter != toRemove.end(); iter++)
	{
	    BasicMemo::storedCalls->erase(*iter);
	}
	
    }
    return DepthBoundedMemo::subTW(S, G);
    
}


