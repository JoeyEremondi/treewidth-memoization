#include "AbstractBottomUp.hh"
#include "VSet.hh"
#include "qset.hh"
#include "AbstractMemo.hh" //TODO better place for constants?
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
	lowerBound = 0;
	/*
	lowerBound = d2degen(S, G);
	std::cout << "d2 lower " << lowerBound << "\n";
	lowerBound = std::max(lowerBound, MMD(S, G));
	std::cout << "MMD lower " << MMD(S, G) << "\n";

	std::cout << "Found lower bound " << lowerBound << "\n";
	*/

	return twForSet(S);
}

int AbstractBottomUp::twForSet(VSet SStart)
{

	VSet emptySet;
	//Initialize our store
	beginLayer(0);
	updateTW(0, emptySet, lowerBound/*NO_WIDTH*/);

	int nSet = SStart.size();


	std::vector<Vertex> vertInfo;

	auto allVertInfo = boost::vertices(G);
	std::vector<Vertex> allVerts(allVertInfo.first, allVertInfo.second);
	//VSet SStart(allVerts);


	SStart.members(vertInfo); // boost::vertices(G);
	auto vertInfoStart = vertInfo.begin();
	auto vertInfoEnd = vertInfo.end();

	//VSet clique;



	int i;

	for (i = 1; i <= nSet; ++i)
	{
		currentLayer = i;
		try {
			std::cout << "Level " << i << "\n";
			//std::cout << "Num Q " << numQCalled << "\n";

			//Initialize our dictionary at this level
			this->beginLayer(i);

			//TODO what is this? Taken from Java version
			int minTW = globalUpperBound;

			//std::unordered_map<VSet, int> currentTW;

			//Store the |Q| values, overwritten each layer
			std::vector<int> qSizes(nGraph);

			for (beginIter(); !iterDone(); iterNext())
			{

				if (r < globalUpperBound)
				{

					//std::cout << "On set " << showSet(S);

					Vertex firstSet = S.first();

					findQvalues(nGraph, S, G, qSizes); //TODO n or nGraph?

					for (auto x = vertInfoStart; x != vertInfoEnd; ++x)
					{
						Vertex v = *x;
						if ((!S.contains(v)) /*&& (!clique.contains(v)) && v < firstSet*/) //TODO check if in clique here?
						{



							VSet SUx = S;
							SUx.insert(v);

							int q = qSizes[v];
							//int trueQ = qCheck(nGraph, S, v, G);
							//int qOld = sizeQ(nGraph, S, v, G);
							//assert(q == trueQ);


							int rr = std::max(r, q);

							//if (rr >= nGraph - i - 1 && rr < minTW) {
							minTW = std::min(minTW, rr);
							globalUpperBound = std::min(globalUpperBound, std::max(minTW, nGraph - i - 1));
							//}

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
		catch (const std::bad_alloc& e) {
			std::cerr << "Allocation failed: " << e.what() << '\n';
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

