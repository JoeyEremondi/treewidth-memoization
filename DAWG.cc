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
			ss << "  " << pair.first << " -> " << pair.second << " [label=\"0 " << i << " \"]\n";
		}
		for (auto pair : delta1[i])
		{
			ss << "  " << pair.first << " -> " << pair.second << " [label=\"1 " << i << " \"]\n";
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
		int ret = pathsToEndFrom(depth + 1, delta(depth, q, true)) + pathsToEndFrom(depth + 1, delta(depth, q, false));
		return ret;
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

	this->delta0 = new std::unordered_map<State, State>[length];
	this->delta1 = new std::unordered_map<State, State>[length];

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

	auto newDelta0 = new std::unordered_map<State, State>[length];
	auto newDelta1 = new std::unordered_map<State, State>[length];

	//Make a product construction with our existing automaton
	std::vector<std::map<std::pair<State, State>, State>> pairMap(length + 1);

	//std::vector<std::set<State>> seenOldSinks(length + 1);
	//State earliestNewSinkSeen;
	//bool haveSetEarliest = false;

	/*
	//Initialize our pair map with our end states
	for (auto transition : delta0[length - 1])
	{
		State repr = newState();
		pairMap[0][{transition.second, newStates[length]}] = repr;
	}
	for (auto transition : delta1[length - 1])
	{
		//Don't double insert
		if (pairMap[0].find({ transition.second, newStates[length] }) == pairMap[0].end())
		{
			State repr = newState();
			pairMap[0][{ transition.second, newStates[length] }] = repr;
		}

	} */

	State initialPair = newState();
	pairMap[0][{initial, newInitial}] = initialPair;

	int totalTransitions = 0;

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

		//Look at all the states with transitions defined the same as our new word
		//Create new states for their pairs, and add them to the map
		for (auto iter = thisDelta[layer].begin(); iter != thisDelta[layer].end(); iter = thisDelta[layer].erase(iter))
		{
			State qFrom = iter->first;
			State qTo = iter->second;

			//Check, are we in the pair map? If not, then we are not reachable from a previous layer
			auto searchInfo = pairMap[layer].find({ qFrom, newFrom });
			if (searchInfo != pairMap[layer].end())
			{
				//Generate an int for our pair state, and store it in the map
				State pairRep = searchInfo->second;

				//See if the state we're going to is in the pair map
				//And if not, create a new state for it
				State nextRep;
				auto nextSearchInfo = pairMap[layer+1].find({ qTo, newTo });
				if (nextSearchInfo == pairMap[layer+1].end())
				{
					nextRep = newState();
					pairMap[layer+1][{ qTo, newTo }] = nextRep;
				}
				else
				{
					nextRep = searchInfo->second;
				}

				//Insert our new transition into our new store
				newDelta[layer][pairRep] = nextRep;
			}

			//Check if we need to add this transition as a successor to a  "sink" transition
			if (pairMap[layer].find({ qFrom, SINK }) != pairMap[layer].end())
			{
				newDelta[layer][qFrom] = qTo;
				pairMap[layer + 1][{qTo, SINK}] = qTo;
			}
		}

		//For each transition qFrom -> b -> qTo, where b doesn't match our new word
		//we send (qFrom, newFrom) to (qTo, SINK), which we represent as qTo to save labels
		//Look at all the states with transitions defined the same as our new word
		//Create new states for their pairs, and add them to the map
		for (auto iter = thatDelta[layer].begin(); iter != thatDelta[layer].end(); iter = thatDelta[layer].erase(iter)) //TODO erase as we go?
		{
			State qFrom = iter->first;
			State qTo = iter->second;

			//Generate an int for our pair state, and store it in the map

			auto searchInfo = pairMap[layer].find({ qFrom, newFrom });
			if (searchInfo == pairMap[layer].end())
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

					//Add this "sink pair" to our map, so we know to add its successors
					pairMap[layer + 1][{SINK, newTo}] = newTo;

				}

				//Use the pair we already assigned
				pairRep = searchInfo->second;

				//Insert our new transition into our new store
				newOtherDelta[layer][pairRep] = qTo;
				//Mark the sink state as seen in our pair map
				pairMap[layer + 1][{qTo, SINK}] = qTo;
			}

			//Check if we need to add this transition as a successor to a  "sink" transition
			if (pairMap[layer].find({ qFrom, SINK }) != pairMap[layer].end())
			{
				newOtherDelta[layer][qFrom] = qTo;
				pairMap[layer + 1][{qTo, SINK}] = qTo;
			}

		}

		//Look at the transition in the automaton accepting our added word
		//And add it if we added the transition before it
		if (pairMap[layer].find({ SINK, newFrom }) != pairMap[layer].end())
		{
			newDelta[layer][newFrom] = newTo;
			//Mark this state as seen, so we add its sucessors
			pairMap[layer + 1][{SINK, newTo}] = newTo;
		}

		//Save memory, we don't ever look back a level
		pairMap[layer].clear();

		//Helps us only minimize when we have to
		totalTransitions += newDelta[layer].size() + newOtherDelta[layer].size();


	}


	//Empty our old deltas
	delete[] delta0;
	delete[] delta1;

	//Set our new delta values
	delta0 = newDelta0;
	delta1 = newDelta1;

	initial = initialPair;




	//When we're done, minimize to save space
	//TODO delete irrelevant vertices?
	if (totalTransitions > 10000) //TODO make this smarter?
	{
		minimize();
	}
}

void DAWG::insert(VSet word)
{

	State currentState = initial;
	for (int i = 0; i < length; ++i)
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
