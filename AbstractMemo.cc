#include "AbstractMemo.hh"
#include "qset.hh"
#include "TWUtilAlgos.hh"

#include <unordered_set>

AbstractMemo::AbstractMemo(Graph theGraph)
{
    G = theGraph;
    numVerts = boost::num_vertices(G);
    
    globalUpperBound = numVerts + 1; //TODO need +1?
    
    storedCalls = new std::map<VSet,int>();
    
}

AbstractMemo::~AbstractMemo()
{
    delete storedCalls;
}

int AbstractMemo::treeWidth()
{
    VSet S(G);
    
    //Optimization from Lemma 11
    //We don't consider any vertices in the max clique
    VSet maxClique = exactMaxClique(G);
    auto cliqueVec = maxClique.members();
    
    for (auto iter = cliqueVec.begin(); iter != cliqueVec.end(); iter++)
    {
	S.erase(*iter);
    }

    std::cout << "Eliminated " << maxClique.size() << " vertices using max clique\n";
    std::cout << "The clique: " << showSet(maxClique) << " \n";

    //Optimiation: set the golbal upper-bound to the TW from some linear ordering
    //TODO choose a smart one?
    globalUpperBound = permutTW(S.members(), G);

    std::cout << "Found global upper-bound of " << globalUpperBound << "\n";
    
    

    return subTW(S);
}



//If a recursive value is stored, then return it
//Otherwise, compute and store it before returning it
int AbstractMemo::fetchOrStore(VSet S) 
{
    //std::cout << storedCalls->size() << "\n";
    
    auto maybeStored = storedCalls->find(S);
    //Did we find the result for set S memoized?
    if ( maybeStored == storedCalls->end())
    {
	//If not, check if we cache it
	//Then calculate it using our recurrence relation
	//And cache it if necessary
	//TODO use DP not recursion for fast case
	memoMisses++;
	if (shouldCache(S))
	{
	    std::pair<VSet, int> newEntry(S, naiveTW(this, S, G));
	    storedCalls->insert(newEntry);
	    return newEntry.second;
	} else {
	    return naiveTW(this, S, G);

	}
	
	
	
    } else {
	memoHits++;
	return maybeStored->second;
    }
    
}

//Override
int AbstractMemo::subTW(VSet S)
{    
    recursionDepth++;
    //std::cout << "Recursion depth " << recursionDepth << "\n";
    //std::cout << "Set " << showSet(S) << "\n";
    
    
    //Clean if we need to
    if (needsCleaning())
    {
	auto toRemove = setsToForget(S);
	for (auto iter = toRemove.begin(); iter != toRemove.end(); iter++)
	{
	    storedCalls->erase(*iter);
	}
	
    }

    
    auto ret = fetchOrStore(S);
    
    recursionDepth--;
    return ret;
    
}


int AbstractMemo::naiveTW(AbstractMemo* memo, VSet S, Graph G)
{
    //std::cout << "NaiveMemo Set: " << showSet(S) << "\n";
    if (S.empty())
    {
        return NO_WIDTH;
    }
    
    auto maybeStored = storedCalls->find(S);

    if (false)//(maybeStored != storedCalls->end())
    {
	return maybeStored->second;
    }
    else
    {
	//Set our minimum so far to the worst possible case for the optimum for this set,
	// or the worst for the graph as a whole, whicever is lower
	//TODO document this 
	//TODO fix this
	//This technically doesn't preserve our recurrence, but it only
	//Gives incorrect results in branches we know will not contribute to the final result
	//Any elements worse than this we don't need to expand
        int minSoFar = std::min(upperBound(S), globalUpperBound);
	//std::cout << "Starting with min " << minSoFar << "\n";
	
	//We let our implementation order the vertices
        auto orderedVerts = memo->orderVertices(S);
	
        for (auto iter = orderedVerts.begin(); iter != orderedVerts.end(); iter++)
        {
            Vertex v = *iter;
            VSet S2(S);
            
            S2.erase(v);
            
            
            int qVal = sizeQ(S2, v, G);

	    //Optimization: if our Q value, or the treewidth lower bound, is worse
	    //than our upper bound so far, don't bother recursively expanding
	    //this option, since it can't be the best
	    if (qVal < minSoFar && lowerBound(S2) < minSoFar)
	    {
		//Count this as expansion
		numExpanded++;
		
		int subTWVal = memo->subTW(S2);
		int thisTW = std::max(subTWVal, qVal );
		minSoFar = std::min(minSoFar, thisTW);
		
		//std::cout << "Current Set: " << showSet(S) << "\n";
		//std::cout << "Try vertex " << v << " subTW " << subTWVal << " qVal " << qVal << "\n";
		
		
		//Update our global upper bound, according to Lemma 9 of the paper
		
		//std::cout << "Current Set: " << showSet(S) << "\n";
		//std::cout << "GUB: old " << globalUpperBound << "\n";
		//std::cout << "Trying " << thisTW << ", " << numVerts - (int)(S.size()) - 1 << "\n";
		
		globalUpperBound = 
		       std::min(globalUpperBound, 
				std::max(thisTW, numVerts - (int)(S.size()) - 1  ) ); 
		
	    }
        }
        return minSoFar;
        
    }
}

void AbstractMemo::printStats()
{
    std::cout << "Num expanded " << numExpanded << std::endl;
    std::cout << "Hits  " << memoHits << std::endl;
    std::cout << "Misses " << memoMisses << std::endl;
    
    
}


