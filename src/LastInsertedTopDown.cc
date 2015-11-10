#include "LastInsertedTopDown.hh"



LastInsertedTopDown::LastInsertedTopDown(const Graph& Gin, int maxBottom, int numHashesIn, int numPerHashIn)
	: AbstractTopDown(G, maxBottom)
	, numHashes(numHashesIn)
	, numPerHash(numPerHashIn)
{
	//Initialize our vectors
	this->memo.resize(numHashes);
}


LastInsertedTopDown::~LastInsertedTopDown()
{
}

bool LastInsertedTopDown::isMemoized(int nSet, VSet S)
{
	int hashVal = std::hash<VSet>()(S) % numHashes;
	auto& ourVec = memo[hashVal];
	for (auto twPair : ourVec)
	{
		if (twPair.first == S)
		{
			retrievedValue = twPair.second;
			return true;
		}
	}
	return false;
}

int LastInsertedTopDown::memoizedValue(int nSet, VSet S)
{
	return retrievedValue;
}

void LastInsertedTopDown::memoizeTW(int nSet, VSet S, int tw)
{
	int hashVal = std::hash<VSet>()(S) % numHashes;
	auto& ourVec = memo[hashVal];
	ourVec.push_front({ S, tw });
	//Only store a fixed number of hash values
	if (ourVec.size() >= numPerHash)
	{
		ourVec.pop_back();
	}
}

void LastInsertedTopDown::cleanMemoized()
{
	std::cerr << "Ran out of memory!\n";
}
