#include "DAWG.hh"
#include "qset.hh"
#include <iostream>
#include <sstream>
#include <algorithm>

std::string DAWG::asDot()
{
	std::ostringstream ss;
	ss << "digraph G {\n";
	for (int i = 0; i < length; i++)
	{
		for (auto pair : delta0[i])
		{
			ss << "  " << pair.first << " -> " << pair.second << " [label=0]\n";
		}
		for (auto pair : delta1[i])
		{
			ss << "  " << pair.first << " -> " << pair.second << " [label=1]\n";
		}
	}
	ss << "}";
	return ss.str();
}

int DAWG::pathsToEndFrom(int depth, State q)
{
	if (q == SINK)
	{
		return 0;
	}
	if (depth == length)
	{
		return 1;
	}
	else
	{
		return pathsToEndFrom(depth + 1, delta(depth, q, true)) + pathsToEndFrom(depth + 1, delta(depth, q, false));
	}
}

State DAWG::newState()
{
	State ret = nextState;
	nextState++;
	return ret;
}

void DAWG::addTransition(int depth, State from, State to, bool readLetter)
{
	//TODO assert not contains already?
	//std::cout << "Adding transition " << from << " ->" << readLetter << "-> " << to << " layer " << depth << "\n" ;
	if (readLetter)
	{
		delta1[depth][from] = to;
	}
	else
	{
		delta0[depth][from] = to;
	}
}

void DAWG::minimizeHelper(int layer, State q)
{
	for (bool bit : { true, false })
	{
		State tnext = delta(layer, q, bit);
		//Don't follow transitions that aren't there
		if (tnext != SINK)
		{
			if (StateMap.find(tnext) == StateMap.end())
			{
				minimizeHelper(layer + 1, tnext);
			}
			//Update our transition to point to the correct vertex
			setTransition(layer, q, bit, StateMap[tnext]);
		}
	}
	if (Register.find(sig(layer, q)) == Register.end())
	{
		StateMap[q] = q;
		Register[sig(layer, q)] = q;
	}
	else
	{
		StateMap[q] = Register[sig(layer, q)];
		deleteState(layer, q);
	}
}

void DAWG::deleteState(int layer, State q)
{
	if (layer < length)
	{
		delta0[layer].erase(q);
		delta1[layer].erase(q);
	}
}


int DAWG::numTransitions()
{
	int ret = 0;

	for (int i = 0; i < length; i++)
	{
		ret += delta0[i].size();
		ret += delta1[i].size();
	}
	return ret;

}

void DAWG::minimize()
{
	//std::cerr << "Transitions before minimization " << numTransitions() << "\n";
	Register.clear();
	StateMap.clear();

	minimizeHelper(0, initial);

	//Finally, adjust our initial vertex if need be
	initial = StateMap[initial];

	Register.clear();
	StateMap.clear();

	//std::cerr << "Transitions after minimization " << numTransitions() << "\n";
}


DAWG::DAWG()
{
	this->length = VSet::maxNumVerts;

	//Allocate our vectors

	this->delta0 = new std::map<State, State>[length];
	this->delta1 = new std::map<State, State>[length];

}

DAWG::~DAWG()
{
	delete[] delta0;
	delete[] delta1;
}

int DAWG::size()
{
	return pathsToEndFrom(0, initial);
}

std::vector<std::string> DAWG::wordSet()
{
	return wordSetHelper(0, initial);
}

std::vector<std::string> DAWG::wordSetHelper(int depth, State q)
{
	std::vector<std::string> v;
	if (q == SINK)
	{
	}
	else if (depth == length)
	{
		v.push_back("");
	}
	else
	{
		for (auto subWord : wordSetHelper(depth + 1, delta(depth, q, true)))
		{
			v.push_back("1" + subWord);
		}
		for (auto subWord : wordSetHelper(depth + 1, delta(depth, q, false)))
		{
			v.push_back("0" + subWord);
		}

		return v;
	}
}

void DAWG::insertSafe(VSet word)
{
	std::cerr << "Start of insert safe\n\n";

	//Special case: if there are no transitions, we just insert the word normally
	if (delta0[0].empty() && delta1[0].empty())
	{
		insert(word);
		return;
	}

	//Make an automaton for our new word
	std::vector<State> newStates(length + 1);
	State newInitial = newState();
	newStates[0] = newInitial;
	for (int i = 0; i < length; ++i)
	{
		State nextState = newState();
		//addTransition(i, currentState, nextState, word.contains(i));
		newStates[i + 1] = nextState;
	}

	std::map<State, State>* newDelta0 = new std::map<State, State>[length];
	std::map<State, State>* newDelta1 = new std::map<State, State>[length];

	//Make a product construction with our existing automaton
	std::map<std::pair<State, State>, State> pairMap;

	std::vector<std::set<State>> seenOldSinks(length+1);
	State earliestNewSinkSeen;
	bool haveSetEarliest = false;

	//Initialize our pair map with our end states
	for (auto transition : delta0[length - 1])
	{
		State repr = newState();
		pairMap[{transition.second, newStates[length]}] = repr;
	}
	for (auto transition : delta1[length - 1])
	{
		//Don't double insert
		if (pairMap.find({ transition.second, newStates[length] }) == pairMap.end())
		{
			State repr = newState();
			pairMap[{ transition.second, newStates[length] }] = repr;
		}
		
	}

	//TODO zero case
	for (int layer = 0; layer < length; ++layer)
	{
		State newFrom = newStates[layer];
		State newTo = newStates[layer + 1];
		bool newBit = word.contains(layer);

		//For each transition matching the letter of our new automaton
		//we add a transition to a new "pair" state
		auto thisDelta = newBit ? delta1 : delta0;
		auto thatDelta = newBit ? delta0 : delta1;

		auto newDelta = newBit ? newDelta1 : newDelta0;
		auto newOtherDelta = newBit ? newDelta0 : newDelta1;

		
		State initialPair = newState();
		pairMap[{initial, newInitial}] = initialPair;

		//Look at all the states with transitions defined the same as our new word
		//Create new states for their pairs, and add them to the map
		for (auto iter = thisDelta[layer].begin(); iter != thisDelta[layer].end(); ++iter)
		{
			State qFrom = iter->first;
			State qTo = iter->second;

			//Check, are we in the pair map? If not, then we are not reachable from a previous layer
			auto searchInfo = pairMap.find({ qFrom, newFrom });
			if (searchInfo != pairMap.end())
			{
				//Generate an int for our pair state, and store it in the map
				State pairRep = searchInfo->second;

				//See if the state we're going to is in the pair map
				//And if not, create a new state for it
				State nextRep;
				auto nextSearchInfo = pairMap.find({ qTo, newTo });
				if (nextSearchInfo == pairMap.end())
				{
					nextRep = newState();
					std::cerr << "Made new state " << nextRep << "\n";
					pairMap[{ qTo, newTo }] = nextRep;
				}
				else
				{
					nextRep = searchInfo->second;
					std::cerr << "Setting next rep " << nextRep << "\n";
				}

				//Insert our new transition into our new store
				newDelta[layer][pairRep] = nextRep;
				std::cerr << "Adding transition for pair (" << qFrom << ", " << newFrom << ") to (" << qTo << ", " << newTo << ")\n";
				std::cerr << pairRep << " -> " << newBit << " -> " << pairMap[{qTo, newTo}] << "\n";
			}
		}

		//For each transition qFrom -> b -> qTo, where b doesn't match our new word
		//we send (qFrom, newFrom) to (qTo, SINK), which we represent as qTo to save labels
		//Look at all the states with transitions defined the same as our new word
		//Create new states for their pairs, and add them to the map
		for (auto iter = thatDelta[layer].begin(); iter != thatDelta[layer].end(); ++iter) //TODO erase as we go?
		{
			State qFrom = iter->first;
			State qTo = iter->second;

			//Generate an int for our pair state, and store it in the map
			
			auto searchInfo = pairMap.find({ qFrom, newFrom });
			if (searchInfo == pairMap.end())
			{
				//Didn't find it, so this pair is not reachable

			}
			else
			{
				State pairRep = searchInfo->second;

				//First, check if we have a transition already defined on newBit
				if (newDelta[layer].find(pairRep) == newDelta[layer].end())
				{
					//If not, we need a sink transition just on the new state
					newDelta[layer][pairRep] = newTo;

					//Mark that we need to add those states to our final DFA
					if (!haveSetEarliest)
					{
						haveSetEarliest = true;
						earliestNewSinkSeen = newTo;
					}
					
				}

				//Use the pair we already assigned
				pairRep = searchInfo->second;

				std::cerr << "Adding transition for pair (" << qFrom << ", " << newFrom << ") to (" << qTo << ", " << SINK << ")\n";
				std::cerr << pairRep << " -> " << newBit << " -> " << qTo << "\n";

				//Insert our new transition into our new store
				newOtherDelta[layer][pairRep] = qTo;
				seenOldSinks[layer + 1].insert(qTo);
			}
			
			
		}


	}



	

	//Traverse all sink states from our original automaton
	//And add transitions as necessary
	//TODO better naming
	for (int layer = 0; layer < length; layer++)
	{
		for (auto sinkState : seenOldSinks[layer])
		{
			
			//TODO avoid recomputing
			State to0 = delta0[layer][sinkState];
			State to1 = delta1[layer][sinkState];

			std::cerr << "layer " << layer  << " Adding sink state main " << sinkState << " 0 -> " << to0 << ", 1 ->" << to1 << "\n";
			if (to1 != SINK)
			{
				std::cerr << "Pushing next state " << to1 << "\n";
				newDelta1[layer][sinkState] =  to1;
				seenOldSinks[layer + 1].insert(to1);
			}

			if (to0 != SINK)
			{
				std::cerr << "Pushing next state " << to0 << "\n";
				newDelta0[layer][sinkState] = to0;
				seenOldSinks[layer + 1].insert(to0);
			}
		}
	}

	//Add all transitions from original DFA for each of our sinkStates
	bool foundFirst = false;
	for (int i = 0; i < length + 1 && !foundFirst; i++)
	{
		if (newStates[i] == earliestNewSinkSeen)
		{
			for (int j = i; j < length; j++)
			{
				auto arr = word.contains(j) ? newDelta1 : newDelta0;
				arr[i][newStates[j]] = newStates[j + 1];
			}
			foundFirst = true;
		}
	}

	//Empty our old deltas
	delete[] delta0;
	delete[] delta1;

	//Set our new delta values
	delta0 = newDelta0;
	delta1 = newDelta1;

	


	//When we're done, minimize to save space
	//TODO delete irrelevant vertices?
	//minimize();

}

void DAWG::insert(VSet word)
{
	static int insertionNum = 0;

	std::cerr << "Start of insert\n\n";

	int sizeBefore = size();
	std::string dotBefore = asDot();

	State currentState = initial;
	for (int i = 0; i < length; ++i)
	{
		State nextState = delta(i, currentState, word.contains(i));
		if (nextState == SINK)
		{

			nextState = newState();
			std::cerr << "Adding new state " << nextState << "leaving " << currentState << " on " << word.contains(i) << "\n";
			addTransition(i, currentState, nextState, word.contains(i));
		}
		currentState = nextState;
	}

	int sizeAfter = size();
	if (sizeBefore + 1 != sizeAfter)
	{
		std::cerr << "Bad size in insert with " << showSet(word) << "\n";
		std::cerr << "before " << sizeBefore << " after " << sizeAfter << "\n";
		std::cerr << dotBefore << "\n\n";
		std::cerr << asDot();
		abort();
	}



	//Minimize as we go, so we never get too large
	insertionNum++;
	if (true /*insertionNum % 10000 == 0*/)
	{
		//TODO reclaim lost state numbers?
		minimize();
		//minimize();
	}
	if (sizeBefore != size())
	{
		abort;
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

	std::vector<State> initPath;
	initPath.push_back(initial);
	iterStack.push_back({ initial, 0, empty, initPath });




}

std::string showStates(std::vector<State> v)
{

	std::ostringstream ss;
	ss << "{";
	for (State s : v)
	{
		ss << s << "; ";
	}
	ss << "}";
	return ss.str();
}

VSet DAWG::nextIter()
{

	bool didFail = true;
	while (didFail)
	{
		didFail = false;
		if (iterStack.empty())
		{
			//std::cout << "Returning empty stack\n";
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

		//std::cout << "Popped " << showSet(currentSet) << " from stack\n";

		std::vector<State> currentPath = top.statePath;

		while (currentPos < length)
		{
			State q0, q1;
			if ((q0 = delta(currentPos, current, false)) != SINK && (q1 = delta(currentPos, current, true)) != SINK)
			{
				//Add the 1 path to our stack
				VSet newSet = currentSet;
				newSet.insert(currentPos);
				std::vector<State> path1 = currentPath;
				path1.push_back(q1);
				iterStack.push_back({ q1, currentPos + 1, newSet, path1 });
				//Keep following the 0 path

				//std::cout << "Pushing " << current << " -> true -> " << q1 << " layer " << currentPos + 1 << "\n";
				//std::cout << "Following " << current << " -> false -> " << q0 << " layer " << currentPos + 1 << "\n";

				current = q0;
				currentSet.erase(currentPos);
				currentPos++;
				currentPath.push_back(q0);

			}
			//If only one path, just follow that path
			else if ((q0 = delta(currentPos, current, false)) != SINK)
			{
				//std::cout << "Following " << current << " -> false -> " << q0 << " layer " << currentPos + 1 << "\n";

				current = q0;
				currentSet.erase(currentPos);
				currentPos++;
				currentPath.push_back(q0);

			}
			else if ((q1 = delta(currentPos, current, true)) != SINK)
			{
				//std::cout << "Following " << current << " -> true -> " << q1 << " layer " << currentPos + 1 << "\n";

				current = q1;
				currentSet.insert(currentPos);
				currentPos++;
				currentPath.push_back(q1);

			}
			else
			{
				//std::cout << "Failing\n";
				didFail = true;
				break;
			}
		}
		//When we reach our set length, return the set we accumulated
		if (!didFail)
		{
			//std::cout << "Returing " << showSet(currentSet) << " after no failures, path " << showStates(currentPath) << "\n";
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
