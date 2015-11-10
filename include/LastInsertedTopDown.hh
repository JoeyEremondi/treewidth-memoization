#pragma once
#include "AbstractTopDown.hh"
#include <unordered_map>
#include <list>

//Instance of AbstractTopDown, where we memoize using a hash table
//Where the chains of the hashtable are of fixed length, and we drop the least-recently
//Accesed vertices of each chain.
class LastInsertedTopDown :
	public AbstractTopDown
{
public:
	LastInsertedTopDown(const Graph& Gin, int maxBottom, int numHashesIn, int numPerHashIn);
	~LastInsertedTopDown();

protected:
	bool isMemoized(int nSet, VSet S);
	int memoizedValue(int nSet, VSet S);
	void memoizeTW(int nSet, VSet S, int tw);
	void cleanMemoized();

	int retrievedValue;

	//TODO make parameters
	int numHashes = 9999991;
	int numPerHash = 100;

	std::vector<std::list<std::pair<VSet, int>>> memo;
};

