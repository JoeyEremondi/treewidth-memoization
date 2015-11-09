#include "AbstractBottomUp.hh"
#include "VSet.hh"
#include "qset.hh"
#include "UpperBound.hh"
#include "TWUtilAlgos.hh"






AbstractBottomUp::AbstractBottomUp(const Graph & Gin)
	: G(Gin)
{
	this->nGraph = boost::num_vertices(G);
	this->globalUpperBound = nGraph;
}

AbstractBottomUp::~AbstractBottomUp()
{
	//Nothing to do, we don't have any heap variables
}

int AbstractBottomUp::tw()
{
	VSet S(G);

	//Remove elements in the max-clique of G
	VSet maxClique = exactMaxClique(G);
	std::vector<Vertex> cliqueVec(maxClique.size());
	maxClique.members(cliqueVec);

	for (auto iter = cliqueVec.begin(); iter != cliqueVec.end(); ++iter)
	{
		S.erase(*iter);
	}

	globalUpperBound = calcUpperBound(G, S);

	//Set the lower bound
	//We don't use this in the bottom-up version, so we just set it to 0
	lowerBound = 0;

	//Run our main algorithm on our set with clique-vertices eliminated
	return twForSet(S);
}

int AbstractBottomUp::twForSet(VSet SStart)
{
	//Used a bunch, just for convenience
	VSet emptySet;

	//Initialize our store
	beginLayer(0);
	
	//Emptyset given our lower-bound value, since we know other values will exceed it
	updateTW(0, emptySet, lowerBound);

	//Keep track of the size of sets we're currently looking at
	int nSet = SStart.size();

	//Vector we use each iteration to store the vertices of our set in a vector for fast iteration
	//We cache the iterators for speed
	std::vector<Vertex> vertInfo;
	SStart.members(vertInfo); // boost::vertices(G);
	auto vertInfoStart = vertInfo.begin();
	auto vertInfoEnd = vertInfo.end();

	//Also cache all the vertices of our graph for speed
	auto allVertInfo = boost::vertices(G);
	std::vector<Vertex> allVerts(allVertInfo.first, allVertInfo.second);

	//Declare this here so we can get at it after the loop
	int i;
	for (i = 1; i <= nSet; ++i)
	{
		currentLayer = i;
		try {

			//Initialize our dictionary at this level
			this->beginLayer(i);

			//We keep track of the smallest TW value we've found in this layer
			//Which is used to refine our upper-bound later
			int minTW = globalUpperBound;

			//Store the |Q| values, overwritten each layer
			std::vector<int> qSizes(nGraph);

			//Look at all Sets and TW values calculated in the last layer
			for (beginIter(); !iterDone(); iterNext())
			{
				//Ignore sets if our lower-bound was refined since we last saw them
				//and they now exceed it
				if (r < globalUpperBound)
				{

					//Find all values of |Q(S, v)| for each vertex in our graph
					findQvalues(nGraph, S, G, qSizes); 

					//For each vertex v, see if we can refine TW(S U v) with the q value 
					for (auto x = vertInfoStart; x != vertInfoEnd; ++x)
					{
						Vertex v = *x;
						if ((!S.contains(v)) ) 
						{
							VSet SUx = S;
							SUx.insert(v);

							int q = qSizes[v];
							int rr = std::max(r, q);

							//Refine our minTW value, and global upper-bound if we can
							minTW = std::min(minTW, rr);
							globalUpperBound = std::min(globalUpperBound, std::max(minTW, nGraph - i - 1));
							

							//If our new value doesn't exceed our upper-bound, set it as the TW value for (S U x)
							//If (S U x) already has a lower value, this has no effect
							if (rr < globalUpperBound && rr >= lowerBound)
							{
								updateTW(i, SUx, rr);
							}

						}

					}
				}


			}


			//De-allocate our old level of the tree, to save space
			endLayer(i - 1);


		}
		catch  (const std::bad_alloc& e) {
			std::cerr << "Allocation failed AbstractBottomUp: " << e.what() << '\n';
			//Free up our resources
			std::cerr << numStored() << " elements in the failing layer\n";
			endLayer(i - 1);
			endLayer(i);
			return -1;
		}

	}

	std::cout << "Finished bottom up with upper bound of " << globalUpperBound << "\n";

	int finalResult = this->finalResult(nSet, SStart);
	endLayer(nSet);
	return finalResult;
}
