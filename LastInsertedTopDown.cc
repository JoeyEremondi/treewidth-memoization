#include "LastInsertedTopDown.hh"



LastInsertedTopDown::LastInsertedTopDown(const Graph& G) 
	: AbstractTopDown(G)
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
	numInDict++;
	//Only store a fixed number of hash values
	if (ourVec.size() >= numPerHash)
	{
		ourVec.pop_back();
		numInDict--;
	}
}

void LastInsertedTopDown::cleanMemoized()
{
	std::cerr << "Ran out of memory!\n";
}
