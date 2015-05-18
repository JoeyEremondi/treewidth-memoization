#include "AbstractMemo.hh"
#include "qset.hh"

#include <set>

AbstractMemo::AbstractMemo(Graph theGraph)
{
    G = theGraph;
    storedCalls = new std::map<std::set<Vertex>,int>();
    
}

AbstractMemo::~AbstractMemo()
{
    delete storedCalls;
}

int AbstractMemo::treeWidth()
{
    auto iterInfo = boost::vertices(G);
    std::set<Vertex> S(iterInfo.first, iterInfo.second);
    return subTW(S);
}



//If a recursive value is stored, then return it
//Otherwise, compute and store it before returning it
int AbstractMemo::fetchOrStore(std::set<Vertex> S) 
{
    auto maybeStored = storedCalls->find(S);
    if ( maybeStored == storedCalls->end())
    {
	if (shouldCache(S))
	{
	    auto newEntry = std::pair<std::set<Vertex>, int>(S, naiveTW(this, S, G));
	    storedCalls->insert(newEntry);
	    return newEntry.second;
	} else {
	    return naiveTW(this, S, G);

	}
	
	
	
    } else {
	return maybeStored->second;
    }
    
}

//Override
int AbstractMemo::subTW(std::set<Vertex> S)
{    
    //Clean if we need to
    if (needsCleaning())
    {
	auto toRemove = setsToForget(S);
	for (auto iter = toRemove.begin(); iter != toRemove.end(); iter++)
	{
	    storedCalls->erase(*iter);
	}
	
    }

    return fetchOrStore(S);
    
}


int AbstractMemo::naiveTW(AbstractMemo* memo, std::set<Vertex> S, Graph G)
{
    if (S.empty())
    {
        return NO_WIDTH;
    }
    
    auto maybeStored = storedCalls->find(S);

    if (maybeStored != storedCalls->end())
    {
	return maybeStored->second;
    }
    else
    {
	//Set our minimum so far to the worst possible case for the optimum
	//Any elements worse than this we don't need to expand
        int minSoFar = upperBound(S);

	//We let our implementation order the vertices
        auto orderedVerts = memo->orderVertices(S);
	
        for (auto iter = orderedVerts.begin(); iter != orderedVerts.end(); iter++)
        {
            Vertex v = *iter;
            std::set<Vertex> S2(S);
            
            S2.erase(v);
            
            int subTW = memo->subTW(S2);
            int qVal = sizeQ(S2, v, G);

	    //Optimization: if our Q value, or the treewidth lower bound, is worse
	    //than our upper bound so far, don't bother recursively expanding
	    //this option, since it can't be the best
	    if (qVal < minSoFar && lowerBound(S2) < minSoFar)
	    {
		minSoFar = std::min(minSoFar, std::max(subTW, qVal ) );
	    }
        }
        return minSoFar;
        
    }
}
