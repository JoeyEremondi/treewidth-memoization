#include "DAWG.hh"
#include "qset.hh"
#include <iostream>

State DAWG::newState()
{
	State ret = nextState;
	nextState++;
	return ret;
}

void DAWG::addTransition(int depth, State from, State to, bool readLetter)
{
	//TODO assert not contains already?
	if (readLetter)
	{
		delta1[depth][from] = to;
	}
	else
	{
		delta0[depth][from] = to;
	}
}

void DAWG::minimize()
{
	//TODO implement
}

DAWG::DAWG()
{
	this->length = VSet::maxNumVerts;
}

void DAWG::insert(VSet word)
{
	State currentState = initial;
	for (int i = 0; i < word.maxNumVerts; ++i)
	{
		State nextState = delta(i, currentState, word.contains(i));
		if (nextState == SINK)
		{
			nextState = newState();
			addTransition(i, currentState, nextState, word.contains(i));
		}
		currentState = nextState;
	}
}

bool DAWG::contains(VSet word)
{
	State currentState = initial;
	for (int i = 0; i < word.maxNumVerts; ++i)
	{
		State nextState = delta(i, currentState, word.contains(i));
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

	if (delta(0, initial, false) != SINK)
	{
		iterStack.push_back({ initial, 0, false, empty });
	}
	if (delta(0, initial, true) != SINK)
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

	State current = top.state;
	int currentPos = top.depth;
	VSet currentSet = top.set;

	while (currentPos < VSet::maxNumVerts - 1)
	{
		if (delta(currentPos, current, false) != SINK && delta(currentPos, current, true) != SINK)
		{
			//Add the 1 path to our stack
			VSet newSet = currentSet;
			newSet.insert(currentPos + 1);
			iterStack.push_back({ delta1[currentPos][current], currentPos + 1, true, newSet });
			//Keep following the 0 path

			current = delta0[currentPos][current];
			currentPos++;
			currentSet.erase(currentPos); //TODO pos or pos-1?
		}
		//If only one path, just follow that path
		else if (delta(currentPos, current, false) != SINK)
		{
			current = delta0[currentPos][current];
			currentPos++;
			currentSet.erase(currentPos); //TODO pos or pos-1?
		}
		else if (delta(currentPos, current, true) != SINK)
		{
			current = delta1[currentPos][current];
			currentPos++;
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

bool DAWG::empty()
{
	return delta(0, initial, false) == SINK && delta(0, initial, true) == SINK;
}
