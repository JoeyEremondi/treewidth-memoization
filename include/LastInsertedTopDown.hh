#pragma once
#include "AbstractTopDown.hh"
#include <unordered_map>
#include <list>

class LastInsertedTopDown :
	public AbstractTopDown
{
public:
	LastInsertedTopDown(const Graph& Gin, int maxBottom) ;
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

