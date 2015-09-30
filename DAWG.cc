#include "DAWG.hh"

State DAWG::newState()
{
	State ret = numStates;
	numStates++;
	return ret;
}

void DAWG::addTransition(int from, int to, bool readLetter)
{
	//TODO assert not contains already?
	if (readLetter)
	{
		delta1[from] = to;
	}
	else
	{
		delta0[from] = to;
	}
}

void DAWG::minimize()
{
	//TODO implement
}

void DAWG::insert(VSet word)
{
	State currentState = initial;
	for (int i = 0; i < word.maxNumVerts; ++i)
	{
		State nextState = delta(currentState, word.contains(i));
		if (nextState == SINK)
		{
			nextState = newState();
			addTransition(currentState, nextState, word.contains(i));
		}
		currentState = nextState;
	}
}

bool DAWG::contains(VSet word)
{
	State currentState = initial;
	for (int i = 0; i < word.maxNumVerts; ++i)
	{
		State nextState = delta(currentState, word.contains(i));
		if (nextState == SINK)
		{
			return false;
		}
		currentState = nextState;
	}
	return true;
}

void DAWG::initIter()
{
	iterStack.clear();

	VSet empty;
	VSet just0;
	just0.insert(0);

	if (delta0[initial] != SINK)
	{
		iterStack.push_back({ initial, 0, false, empty });
	}
	if (delta1[initial] != SINK)
	{
		iterStack.push_back({ initial, 0, true, just0 });
	}


}

VSet DAWG::nextIter()
{
	//Pop an element off the stack
	//Then find a path to an end-state, adding alternate
	//paths to the stack as we go
	auto top = iterStack.back();
	iterStack.pop_back();

	int current = top.state;
	int currentPos = top.depth;
	VSet currentSet = top.set;

	while (currentPos < VSet::maxNumVerts)
	{
		if (delta0[current] != SINK && delta1[current] != SINK)
		{
			//Add the 1 path to our stack
			VSet newSet = currentSet;
			newSet.insert(currentPos + 1);
			iterStack.push_back({ delta1[current], currentPos + 1, true, newSet });
			//Keep following the 0 path
			currentPos++;
			current = delta0[current];
			currentSet.erase(currentPos); //TODO pos or pos-1?
		}
		//If only one path, just follow that path
		else if (delta0[current] != SINK)
		{
			currentPos++;
			current = delta0[current];
			currentSet.erase(currentPos); //TODO pos or pos-1?
		}
		else if (delta1[current] != SINK)
		{
			currentPos++;
			current = delta1[current];
			currentSet.erase(currentPos); //TODO pos or pos-1?
		}
		//TODO check that all strings same length?
	}
	//When we reach our set length, return the set we accumulated
	return currentSet;
}

bool DAWG::iterDone()
{
	return iterStack.empty();
}
