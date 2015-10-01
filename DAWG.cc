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
	//std::cerr << "Adding transition " << from << " ->" << readLetter << "-> " << to << " layer " << depth << "\n" ;
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
	this->delta0.resize(length);
	this->delta1.resize(length);
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

	iterStack.push_back({ initial, 0, empty });




}

VSet DAWG::nextIter()
{

	bool didFail = true;
	while (didFail)
	{
		didFail = false;
		if (iterStack.empty())
		{
			//std::cerr << "Returning empty stack\n";
			VSet ret;
			return ret;
		}

		//Pop an element off the stack
		//Then find a path to an end-state, adding alternate
		//paths to the stack as we go
		auto top = iterStack.back();
		iterStack.pop_back();

		State current = top.state;
		int currentPos = top.depth;
		VSet currentSet = top.set;

		while (currentPos < length)
		{
			State q0, q1;
			if ((q0 = delta(currentPos, current, false)) != SINK && (q1 = delta(currentPos, current, true)) != SINK)
			{
				//Add the 1 path to our stack
				VSet newSet = currentSet;
				newSet.insert(currentPos + 1);
				iterStack.push_back({ q1, currentPos + 1, newSet });
				//Keep following the 0 path

				//std::cerr << "Pushing " << current << " -> true -> " << q1 << " layer " << currentPos+1 << "\n";
				//std::cerr << "Following " << current << " -> false -> " << q0 << " layer " << currentPos + 1 << "\n";

				current = q0;
				currentSet.erase(currentPos);
				currentPos++;
				
			}
			//If only one path, just follow that path
			else if ((q0 = delta(currentPos, current, false)) != SINK)
			{
				//std::cerr << "Following " << current << " -> false -> " << q0 << " layer " << currentPos + 1 << "\n";

				current = q0;
				currentSet.erase(currentPos);
				currentPos++;
				
			}
			else if ((q1 = delta(currentPos, current, true)) != SINK)
			{
				//std::cerr << "Following " << current << " -> true -> " << q1 << " layer " << currentPos + 1 << "\n";

				current = q1;
				currentSet.insert(currentPos);
				currentPos++;
				
			}
			else
			{
				//std::cerr << "Failing\n";
				didFail = true;
				break;
			}
		}
		//When we reach our set length, return the set we accumulated
		if (!didFail)
		{
			//std::cerr << "Returing " << showSet(currentSet) << " after no failures\n";
			return currentSet;
		}

	}
}

bool DAWG::iterDone()
{
	return iterStack.empty();
}

bool DAWG::empty()
{
	return delta(0, initial, false) == SINK && delta(0, initial, true) == SINK;
}
