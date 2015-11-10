#ifndef __BOTTOM__UP__TW__H__
#define __BOTTOM__UP__TW__H__

#include "qset.hh"
#include "TWUtilAlgos.hh"

//Similar to AbstractBottomUp
//But it stops when it runs out of memory or hits a transition limit
//And keeps the values it stored, for use in a top-down algorithm
class BottomUpTW
{
public:
	BottomUpTW(const Graph& gIn, int maxLayerSize);
	inline bool foundSolution() { return didFindSolution; }
	inline int solution() { return twValueFound; }
	inline int levelReached() { return maxLevelReached; };
	inline std::map<VSet, int>* topLevelDict() { return topLevelDictFound; };
	inline int bestUpperBound() { return upperBound; }

private:
	void fillTWSet(const VSet& SStart);
	int maxLayerSize;
	const Graph& G;
	int maxLevelReached = 0;
	bool didFindSolution = false;
	int twValueFound;
	int upperBound;
	std::map<VSet, int>* topLevelDictFound;


};

#endif