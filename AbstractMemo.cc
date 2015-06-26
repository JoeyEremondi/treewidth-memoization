#include "AbstractMemo.hh"
#include "qset.hh"
#include "TWUtilAlgos.hh"

#include <unordered_set>

AbstractMemo::AbstractMemo(Graph theGraph)
{
    G = theGraph;
    GC = complement_graph(G).first;

    int n = boost::num_vertices(G);
    
    adjMatrix = new bool*[n];
    
    for (int i = 0; i < n; i++)
    {
	adjMatrix[i] = new bool[n];
    }
    
    

    for (int i = 0; i < n; i++)
    {
	for (int j = 0; j < n; j++)
	{
	    //TODO accurate numbering?
	    adjMatrix[i][j] = boost::edge(i, j, G).second;
	    adjMatrix[j][i] = adjMatrix[i][j];
	}
    } 
    
    
    numVerts = boost::num_vertices(G);
    
    globalUpperBound = numVerts + 1; //TODO need +1?
    
    //storedCalls = new std::map<VSet,int>();
    storedCalls = new std::unordered_map<VSet,int>();
    
}

AbstractMemo::~AbstractMemo()
{
    delete storedCalls;
    delete adjMatrix;
}

int AbstractMemo::treeWidth()
{
    VSet Sall(G);
    VSet S = Sall;
    
    //Optimization from Lemma 11
    //We don't consider any vertices in the max clique
    maxClique = exactMaxClique(G);
    std::vector<Vertex> cliqueVec;
    maxClique.members(cliqueVec);
    
    for (auto iter = cliqueVec.begin(); iter != cliqueVec.end(); iter++)
    {
	S.erase(*iter);
    }

    std::cout << "Eliminated " << maxClique.size() << " vertices using max clique\n";
    std::cout << "The clique: " << showSet(maxClique) << " \n";

    setGlobalUpperBound(S);
    //We want all the vertex info to make our lower bound as high as we can
    setGlobalLowerBound(Sall);
    

    //std::cout << "Found global upper-bound of " << globalUpperBound << "\n";
    
    

    return subTW(lowerBound(S), globalUpperBound, S);
}

void AbstractMemo::setGlobalUpperBound(VSet S)
{
    //Optimiation: set the golbal upper-bound to the TW from some linear ordering
    //First try: default ordering
    std::vector<Vertex> Svec;
    S.members(Svec);
    
    globalUpperBound = permutTW(S, Svec, G);
    
    //Second try: order by degree ascending
    Graph gthis = this->G;
    std::sort(Svec.begin(), Svec.end(), [this](Vertex u, Vertex v)
			    {
				auto ud = boost::degree(u, this->G);
				auto vd = boost::degree(v, this->G);
				return ud < vd;
			    } );
    globalUpperBound = std::min(globalUpperBound, permutTW(S, Svec, G));

    //Third try: order by degree descending
    //Svec = S.members();
    std::sort(Svec.begin(), Svec.end(), [this](Vertex u, Vertex v)
			    {
				auto ud = boost::degree(u, this->G);
				auto vd = boost::degree(v, this->G);
				return ud > vd;
			    } );
    globalUpperBound = std::min(globalUpperBound, permutTW(S, Svec, G)); 

    std::cout << "Found global upper bound " << globalUpperBound << "\n";
}

void AbstractMemo::setGlobalLowerBound(VSet S)
{
    //Try 1: The treewidth is at least the size of the largest clique
    //TODO +1?
    globalLowerBound = maxClique.size();
    
    globalLowerBound = std::max(globalLowerBound, MMD(S, G));

    std::cout << "Found global lower bound " << globalLowerBound << "\n";
    
}





//If a recursive value is stored, then return it
//Otherwise, compute and store it before returning it
int AbstractMemo::fetchOrStore(int lowerBound, int upperBound, const VSet& S) 
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
	if (true)//(shouldCache(S))
	{
	    int twVal = naiveTW( lowerBound, upperBound, S);
	    std::pair<VSet, int> newEntry(S, twVal);
	    (*storedCalls)[S] = twVal;
	    return newEntry.second;
	} else {
	    return naiveTW(lowerBound, upperBound, S);

	}
	
	
	
    } else {
	memoHits++;
	return maybeStored->second;
    }
    
}

//Override
int AbstractMemo::subTW(int lowerBound, int upperBound, const VSet& S)
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

    
    auto ret = fetchOrStore(lowerBound, upperBound, S);
    
    recursionDepth--;
    return ret;
    
}


int AbstractMemo::naiveTW(int ourLB, int ourUB, const VSet& S)
{
    /*
    if (S.asInt() % 2047 == 0)
    {
	std::cout << "Stored values " << storedCalls->size() << "\n";
	} */
    

    //std::cout << "NaiveMemo Set: " << showSet(S) << "\n";
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
	std::vector<Vertex> orderedVerts;
	S.members(orderedVerts);
	orderVertices(orderedVerts);

        for (auto iter = orderedVerts.begin(); iter != orderedVerts.end(); iter++)
        {
	    //Optimization: if our upper bound and lower bound converge, then return them
	    if (minSoFar == ourLB)
	    {
		return minSoFar;
	    }

            Vertex v = *iter;
            VSet S2(S);
            
            S2.erase(v);
            
            
            int qVal = sizeQ(S2, v, G);

	    //Optimization: if our Q value, or the treewidth lower bound, is worse
	    //than our upper bound so far, don't bother recursively expanding
	    //this option, since it can't be the best
	    int subLB = 0;
	    int subUB = boost::num_vertices(G);
	    int thisTW;
	    if (qVal < minSoFar && (subLB = lowerBound(S2)) < minSoFar  )
	    {	
		/*
		subUB = upperBound(S2);
		
		//Optimization: the upper bound of the recursive value
		//is less than our local or global lower bound,
		//We know the value doesn't contribute to our final value
		
		if (globalLowerBound > subUB)
		{
		    //TODO why doesn't this case work?
		    std::cout << "GLB > SUB \n" << globalLowerBound << " " << subUB << "\n";
 
		    return -1;//globalLowerBound;
		} 
		
		if (qVal > subUB  || ourLB > subUB )//(ourLB > subUB || globalLowerBound > subUB)
		{
		    std::cout << "Qval vs sub UB" << ourLB << " " << subUB << "\n";
		    thisTW = qVal;
		} else {
		    */
		    //Count this as expansion
		    numExpanded++;
		    int subTWVal = subTW(subLB, subUB, S2);
		    thisTW = std::max(subTWVal, qVal );
		    //}
		minSoFar = std::min(minSoFar, thisTW);	    
		//Adjust our global upper bound according to the lemma
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


