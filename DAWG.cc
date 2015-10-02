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

	for (auto transMap : delta0)
	{
		ret += transMap.size();
	}
	for (auto transMap : delta1)
	{
		ret += transMap.size();
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

/*
//We make assumptions about our DFA:
//All accepted words are the same length
//Transitions in layers, only from layer i to layer i+1
//All states in final layer are accepting
//We also assume all states are reachable, and can reach some final state
void DAWG::minimize()
{
	//Step 1 : Determine which vertices are distinct
	//see https://www.cs.umd.edu/class/fall2009/cmsc330/lectures/discussion2.pdf


	std::set<State> empty;
	std::vector<State> equivClasses(nextState, SINK);

	//Each final state is equivalent to the smallest final state
	std::set<State> allFinalStates;
	for (auto arr : { delta0, delta1 })
	{
		for (auto pair : arr[length - 2])
		{
			allFinalStates.insert(pair.second);
		}
	}
	State finalEquivClass = *(std::min_element(allFinalStates.begin(), allFinalStates.end()));
	for (State f : allFinalStates)
	{
		equivClasses[f] = finalEquivClass;
	}


	//For each distance from the start vertex, starting with our final states
	for (int i = length - 2; i >= 0; --i)
	{
		//For each vertex, keep the list of larger vertices which are distinct from it
		//Vertices at different distances from the start state are assumed distinct
		std::map<State, std::set<State>> distinct;

		//First, we collect the list of vertices in this layer
		std::set<State> thisLayerAllStates;
		for (auto arr : { delta0, delta1 })
		{
			for (auto pair1 : arr[i])
			{
				thisLayerAllStates.insert(pair1.first);
			}

		}

		//Then, we look at all pairs and see if they are distinct
		for (State q1 : thisLayerAllStates)
		{
			//Initialize the distinct set for each vertex
			distinct[q1] = empty;
			//Each state initially is in its own equivalence class
			equivClasses[q1] = q1;

			for (State q2 : thisLayerAllStates)
			{
				//Check if they disagree on either transition, after applying found equivalences for the previous layer
				for (bool bit : {true, false})
				{
					if (equivClasses[delta(i, q1, bit)] != equivClasses[delta(i, q2, bit)])
					{
						distinct[q1].insert(q2);
					}
				}
			}
		}
		//Now that we have a complete list of distinct vertices, we set the equivalence class
		//of each vertex as the smallest vertex not distinct from it
		for (State q1 : thisLayerAllStates)
		{
			for (State q2 : thisLayerAllStates)
			{
				if (q1 < q2 && distinct[q1].find(q2) == distinct[q1].end())
				{
					equivClasses[q2] = std::min(equivClasses[q2], q1);
				}
			}
		}
	}

	//Now, we have a vector mapping each vertex to its equivalence class
	//So we go through our maps delta1 and delta0, delete redundant vertices
	//And redirect transitions to their equivalence classes
	for (auto arr : { delta0, delta1 })
	{
		for (int layer = 0; layer < length; layer++)
		{
			//Iterate, possibly deleting as we go
			auto iter = arr[layer].begin();
			while (iter != arr[layer].end())
			{
				State from = iter->first;
				State to = iter->second;
				if (from == equivClasses[from])
				{
					//Update our transition to point to the new state
					arr[layer][from] = equivClasses[to];
					++iter;
				}
				else
				{
					//This transition is irrelevant, remove it
					iter = arr[layer].erase(iter);
				}
			}
		}
	}


	//Finally, adjust our initial vertex if need be
	//initial = equivClasses[initial]; //TODO make not const?
}
*/

DAWG::DAWG()
{
	this->length = VSet::maxNumVerts;
	this->delta0.resize(length);
	this->delta1.resize(length);
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
	std::cerr << "Start of insert\n\n";

	//Make an automaton for our new word
	std::vector<State> newStates(length + 1);
	State currentState = initial;
	newStates[0] = currentState;
	for (int i = 0; i < length; ++i)
	{
		State nextState = delta(i, currentState, word.contains(i));
		nextState = newState();
		//addTransition(i, currentState, nextState, word.contains(i));
		newStates[i + 1] = nextState;
		currentState = nextState;
	}

	std::vector<std::map<State, State>> newDelta0;
	std::vector<std::map<State, State>> newDelta1;

	//Make a product construction with our existing automaton
	std::map<std::pair<State, State>, State> pairMap;

	std::vector<std::set<State>> seenOldSinks(length);
	State earliestNewSinkSeen;

	//TODO zero case
	for (int layer = length - 1; layer >= 0; --length)
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

			//Generate an int for our pair state, and store it in the map
			State pairRep = newState();
			pairMap[{qFrom, newFrom}] = pairRep;

			//Insert our new transition into our new store
			newDelta[layer][pairRep] = pairMap[{qTo, newTo}];
		}

		//For each transition qFrom -> b -> qTo, where b doesn't match our new word
		//we send (qFrom, newFrom) to (qTo, SINK), which we represent as qTo to save labels
		//Look at all the states with transitions defined the same as our new word
		//Create new states for their pairs, and add them to the map
		for (auto iter = thatDelta[layer].begin(); iter != thatDelta[layer].end(); iter = thatDelta[layer].erase(iter))
		{
			State qFrom = iter->first;
			State qTo = iter->second;

			//Generate an int for our pair state, and store it in the map
			State pairRep;
			auto searchInfo = pairMap.find({ qFrom, newFrom });
			if (searchInfo == pairMap.end())
			{
				//Didn't find it, so we need a new representative for this pair
				pairRep = newState();
				//Also means qFrom has no transition on bit, so we add one to a sink state
				//again, representing (SINK, newTo) as newTo
				newDelta[layer][newFrom] = newTo;
				earliestNewSinkSeen = newStates[layer];

			}
			else
			{
				//Use the pair we already assigned
				pairRep = searchInfo->second;
			}
			pairMap[{qFrom, newFrom}] = pairRep;

			//Insert our new transition into our new store
			newOtherDelta[layer][pairRep] = qTo;
			seenOldSinks[layer].insert(qTo);
		}


	}

	//Empty our old deltas
	delta0.clear();
	delta1.clear();

	//Set our new delta values
	//TODO pointers?
	delta0 = newDelta0;
	delta1 = newDelta1;

	//Add all transitions from original DFA for each of our sinkStates
	bool foundFirst = false;
	for (int i = 0; i < length + 1 && !foundFirst; i++)
	{
		if (newStates[i] == earliestNewSinkSeen)
		{
			for (int j = i; j < length; j++)
			{
				addTransition(i, newStates[i], newStates[i + 1], word.contains(i));
			}
			foundFirst = true;
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
			State to0;
			State to1;
			if ((to1 =delta(layer, sinkState, true)) != SINK)
			{
				addTransition(layer, sinkState, true, to1);
				seenOldSinks[layer + 1].insert(to1);
			}
			if ((to0 = delta(layer, sinkState, false)) != SINK)
			{
				addTransition(layer, sinkState, false, to0);
				seenOldSinks[layer + 1].insert(to0);
			}
		}
	}


	//When we're done, minimize to save space
	//TODO delete irrelevant vertices?
	minimize();

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
