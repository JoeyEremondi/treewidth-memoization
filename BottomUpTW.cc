#include "BottomUpTW.hh"

#include "UpperBound.hh"

#include <algorithm>
#include <iostream>
#include <string>
#include <sstream> // for ostringstream
#include <cassert>


int bottomUpTW(const Graph& G)
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

	int upperBound = calcUpperBound(G, S);

	return bottomUpTWFromSet(G, S, upperBound);



}



std::string showLayer(std::unordered_map<VSet, int> TW)
{
	std::ostringstream out;
	for (auto iter = TW.begin(); iter != TW.end(); ++iter)
	{
		out << "{" << showSet(iter->first) << "=" << iter->second << "}, ";
	}
	return out.str();
}

/*
#if defined (__GNUC__) && ( __GNUC__ >= 3 )
typedef __gnu_cxx::hash_map< HashKey,
#else
typedef std::hash_map< HashKey,
#endif
unsigned,
hash_compare_for_pairs,
hash_equalkey_for_pairs > VertexSetHashMap;
*/

int maybeMin(int x, int y)
{
	if (x == -1)
	{
		return y;
	}
	if (y == -1)
	{
		return x;
	}
	return std::min(x, y);
}

int bottomUpTWFromSet(const Graph& G, const VSet& SStart, int globalUpperBound)
{
	auto TW = new std::unordered_map<VSet, int>*[VSet::maxNumVerts];

	VSet emptySet;
	TW[0] = new std::unordered_map<VSet, int>();
	(*TW[0])[emptySet] = NO_WIDTH;

	//TODO remove
	//globalUpperBound = 26;


	int nGraph = boost::num_vertices(G);
	//int n = nGraph;
	int nSet = SStart.size();


	std::vector<Vertex> vertInfo;

	auto allVertInfo = boost::vertices(G);
	std::vector<Vertex> allVerts(allVertInfo.first, allVertInfo.second);
	//VSet SStart(allVerts);


	SStart.members(vertInfo); // boost::vertices(G);
	auto vertInfoStart = vertInfo.begin();
	auto vertInfoEnd = vertInfo.end();

	//VSet clique;



	int upperBound = globalUpperBound;

	for (int i = 1; i <= nSet; ++i)
	{
		std::cout << "Level " << i << "\n";
		//std::cout << "Num Q " << numQCalled << "\n";

		//Initialize our dictionary at this level
		TW[i] = new std::unordered_map<VSet, int>();

		//TODO what is this? Taken from Java version
		int minTW = upperBound;

		//std::unordered_map<VSet, int> currentTW;

		//Store the |Q| values, overwritten each layer
		std::vector<int> qSizes(nGraph);

		auto twLoopEnd = TW[i - 1]->end();
		for (auto pair = TW[i - 1]->begin(); pair != twLoopEnd; ++pair)
		{
			VSet S = pair->first;
			int r = pair->second;

			if (r < upperBound)
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
						upperBound = std::min(upperBound, std::max(minTW, nGraph - i - 1));
						//}

						if (rr < upperBound)
						{
							auto searchInfo = TW[i]->find(SUx);
							if (searchInfo == TW[i]->end() || rr < searchInfo->second)
							{
								//std::vector<Vertex> newSeq(oldSeq);
								//newSeq.push_back(v);
								(*TW[i])[SUx] = rr;
							}


						}
					}

				}
			}


		}



		//De-allocate our old level of the tree, to save space
		delete TW[i - 1];

		std::cout << "TW i size: " << i << " " << TW[i]->size() << "\n";
		/*if (true)
		{

			std::cout << "TW i: " << i << " " << showLayer(TW[i]) << "\n";
		} */
	}

	//std::cout << "Num Q " << numQCalled << "\n";



	auto searchInfo = TW[nSet]->find(SStart);
	if (searchInfo == TW[nSet]->end())
	{
		delete TW[nSet];
		delete[] TW;
		return upperBound;
	}
	delete TW[nSet];
	delete[] TW;
	return searchInfo->second;



}

BottomUpTW::BottomUpTW(const Graph & gIn, int maxLayerSizeIn)
	: G(gIn)
{
	maxLayerSize = maxLayerSizeIn;

	VSet S(gIn);

	//Remove elements in the max-clique of G
	VSet maxClique = exactMaxClique(gIn);
	std::vector<Vertex> cliqueVec(maxClique.size());
	maxClique.members(cliqueVec);

	for (auto iter = cliqueVec.begin(); iter != cliqueVec.end(); ++iter)
	{
		S.erase(*iter);
	}

	upperBound = calcUpperBound(G, S);

	this->fillTWSet(S);

}

void BottomUpTW::fillTWSet(const VSet& SStart)
{
	auto TW = new std::unordered_map<VSet, int>*[VSet::maxNumVerts];

	VSet emptySet;
	TW[0] = new std::unordered_map<VSet, int>();
	(*TW[0])[emptySet] = NO_WIDTH;

	//TODO remove
	//globalUpperBound = 26;


	int nGraph = boost::num_vertices(G);
	//int n = nGraph;
	int nSet = SStart.size();


	std::vector<Vertex> vertInfo;

	auto allVertInfo = boost::vertices(G);
	std::vector<Vertex> allVerts(allVertInfo.first, allVertInfo.second);
	//VSet SStart(allVerts);


	SStart.members(vertInfo); // boost::vertices(G);
	auto vertInfoStart = vertInfo.begin();
	auto vertInfoEnd = vertInfo.end();

	//VSet clique;

	

	for (int i = 1; i <= nSet; ++i)
	{
		//Make sure our dict doesn't get too big
		int numStoredElements = 0;

		//Initialize our dictionary at this level
		TW[i] = new std::unordered_map<VSet, int>();

		int minTW = upperBound;

		//Store the |Q| values, overwritten each layer
		std::vector<int> qSizes(nGraph);

		auto twLoopEnd = TW[i - 1]->end();
		for (auto pair = TW[i - 1]->begin(); pair != twLoopEnd; ++pair)
		{
			VSet S = pair->first;
			int r = pair->second;

			if (r < upperBound)
			{

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


						int rr = std::max(r, q);

						minTW = std::min(minTW, rr);
						upperBound = std::min(upperBound, std::max(minTW, nGraph - i - 1));

						if (rr < upperBound)
						{
							auto searchInfo = TW[i]->find(SUx);
							if (searchInfo == TW[i]->end())
							{
								//Create new element in our of stored values, if we have space
								if (numStoredElements < maxLayerSize)
								{
									(*TW[i])[SUx] = rr;
									numStoredElements++;
								}
								else //If we run out of space, store the last dict we fully calculated
								{
									this->didFindSolution = false;
									this->twValueFound = NO_WIDTH;
									this->maxLevelReached = i - 1;
									this->topLevelDictFound = TW[i - 1];
									delete TW[i];
									delete TW;
									return;
								}
								
							}
							else if (rr < searchInfo->second)
							{
								(*TW[i])[SUx] = rr;
							}


						}
					}

				}
			}


		}

		std::cout << "TW i size: " << i << " " << TW[i]->size() << "\n";

		//De-allocate our old level of the tree, to save space
		delete TW[i - 1];

	}

	//std::cout << "Num Q " << numQCalled << "\n";

	//If we got to this point without failing, then we found a solution
	didFindSolution = true;

	auto searchInfo = TW[nSet]->find(SStart);
	if (searchInfo == TW[nSet]->end())
	{
		twValueFound = upperBound;
	}
	else
	{
		twValueFound = searchInfo->second;
	}
	//Delete our array, and the last layer, we don't need it if we found a solution
	delete TW[nSet];
	delete[] TW;



}
