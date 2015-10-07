#include "DAWG.hh"
#include "qset.hh"
#include <iostream>
#include <sstream>
#include <algorithm>

#include "AbstractMemo.hh"

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
	for (auto pair : *valueDelta)
	{
		ss << "  " << pair.first << " -> " << FINAL << " [label=\"" << pair.second << " \"]\n";
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

State DAWG::minimizeHelper(int layer, State q)
{
	auto& reg = Register[layer];
	auto& sMapPlus1 = StateMap[layer + 1];
	auto sEndPlus1 = StateMapEnd[layer + 1];

	if (layer == length) //Transitions are on our possible TW values
	{
		int twVal = (*valueDelta)[q];
		//We don't need to traverse further, we know all these states lead to the final state
		//after reading a TW value

		auto searchInfo = EndRegister.emplace(twVal, q);
		if (searchInfo.second)
		{
			StateMap[layer].emplace(q, q);
			return q;
		}
		else
		{
			//It was already in the register, so we just return what's stored in the state map
			State repr = searchInfo.first->second;
			StateMap[layer].emplace(q, repr);
			deleteState(layer, q);
			return repr;
		}

	}

	State tnext0;
	State tnext1;

	//Look at our true transition
	auto tnextIter = delta1[layer].find(q); //delta(layer, q, bit);
	//Don't follow transitions that aren't there
	if (tnextIter != d1end[layer])
	{
		tnext1 = tnextIter->second;
		auto nextSearchInfo = sMapPlus1.find(tnext1);
		if (nextSearchInfo == sEndPlus1)
		{
			State nextRepr = minimizeHelper(layer + 1, tnext1);
			tnextIter->second = nextRepr;
			//setTransition(layer, q, bit, nextRepr);
		}
		else
		{
			tnextIter->second = nextSearchInfo->second;
			//setTransition(layer, q, bit, nextSearchInfo->second);
		}
		//Update our transition to point to the correct vertex

	}
	else
	{
		tnext1 = SINK;
	}

	//Do the same for false
	tnextIter = delta0[layer].find(q); //delta(layer, q, bit);
	//Don't follow transitions that aren't there
	if (tnextIter != d0end[layer])
	{
		tnext0 = tnextIter->second;
		auto nextSearchInfo = sMapPlus1.find(tnext0);
		if (nextSearchInfo == sEndPlus1)
		{
			State nextRepr = minimizeHelper(layer + 1, tnext0);
			tnextIter->second = nextRepr;
			//setTransition(layer, q, bit, nextRepr);
		}
		else
		{
			tnextIter->second = nextSearchInfo->second;
			//setTransition(layer, q, bit, nextSearchInfo->second);
		}
		//Update our transition to point to the correct vertex

	}
	else
	{
		tnext0 = SINK;
	}




	StateSignature ourSig = { tnext0, tnext1 };
	auto searchInfo = reg.emplace(ourSig, q);
	if (searchInfo.second)
	{
		StateMap[layer].emplace(q, q);
		return q;
	}
	else
	{
		State repr = searchInfo.first->second;
		StateMap[layer].emplace(q, repr);
		deleteState(layer, q);
		return repr;
	}
}

void DAWG::deleteState(int layer, State q)
{
	if (layer < length)
	{
		delta0[layer].erase(q);
		delta1[layer].erase(q);
	}
	else
	{
		valueDelta->erase(q);
	}
}


void DAWG::clear()
{
	for (int i = 0; i < length; i++)
	{
		delta0[i].clear();
		delta1[i].clear();
	}
	valueDelta->clear();
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

void DAWG::trim()
{
	//First layer: delete transitions to NO_TW
	auto transitionIter = valueDelta->begin();
	while (transitionIter != valueEnd)
	{
		if (transitionIter->second == NO_WIDTH)
		{
			transitionIter = valueDelta->erase(transitionIter);
		}
		else
		{
			++transitionIter;
		}
	}

	//Second layer: delete transitions that point to something not in ValueDelta
	for (auto arr : { delta0, delta1 })
	{
		auto currentDelta = arr[length];
		auto iter = currentDelta.begin();
		auto loopEnd = currentDelta.end();
		while (iter != loopEnd)
		{
			if (valueDelta->find(iter->second) == valueEnd)
			{
				iter = currentDelta.erase(iter);
			}
			else
			{
				++iter;
			}
		}
	}

	//Then, delete every transition that points to an already deleted transition
	for (int i = length - 2; i >= 0; --i)
	{
		for (auto arr : { delta0, delta1 })
		{
			auto currentDelta = arr[i];
			auto iter = currentDelta.begin();
			auto loopEnd = currentDelta.end();


			auto d0next = delta0[i + 1];
			auto d0nextEnd = d0end[i + 1];
			auto d1next = delta1[i + 1];
			auto d1nextEnd = d1end[i + 1];

			auto nextEnd = arr[i + 1].end();
			while (iter != loopEnd)
			{
				//Check if either transition from this state can reach the end
				if (d0next.find(iter->second) == d0nextEnd && d1next.find(iter->second) == d1nextEnd)
				{
					//If not, delete this state
					iter = currentDelta.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}
	}


	//After we've trimmed, minimize
	this->minimize();
}

void DAWG::minimize()
{
	//std::cerr << "Transitions before minimization " << numTransitions() << "\n";
	Register = new std::unordered_map<StateSignature, State>[length + 1];
	StateMap = new std::unordered_map<State, State>[length + 1];
	StateMapEnd = new std::unordered_map<State, State>::iterator[length + 1];

	//EndRegister.clear();

	for (int i = 0; i <= length; ++i)
	{
		StateMapEnd[i] = StateMap[i].end();
	}
	EndRegisterEnd = EndRegister.end();

	minimizeHelper(0, initial);

	//Finally, adjust our initial vertex if need be
	initial = StateMap[0][initial];

	delete[] Register;
	delete[] StateMap;
	delete[] StateMapEnd;

	EndRegister.clear();

	//Register.clear();
	//StateMap.clear();

	//std::cerr << "Transitions after minimization " << numTransitions() << "\n";
}


DAWG::DAWG()
{
	this->length = VSet::maxNumVerts;

	//Allocate our vectors

	this->delta0 = new std::unordered_map<State, State>[length];
	this->delta1 = new std::unordered_map<State, State>[length];
	this->valueDelta = new std::unordered_map<State, int>;

	setIterEnds();

}

DAWG::DAWG(const DAWG & that)
{
	//Initialize length and pointers
	this->length = that.length;
	this->delta0 = new std::unordered_map<State, State>[length];
	this->delta1 = new std::unordered_map<State, State>[length];
	this->valueDelta = new std::unordered_map<State, int>;

	//Copy delta0 and delta1 from input
	for (int i = 0; i < length; i++)
	{
		delta0[i] = that.delta0[i];
		delta1[i] = that.delta1[i];
	}

	//Point each entry to NO_WIDTH in ValueDelta to show that they're from an old level
	for (auto transition : *that.valueDelta)
	{
		valueDelta->emplace(transition.first, NO_WIDTH);
	}

	//Initialize iter ends
	setIterEnds();

	minimize(); //There could be lots of redundancy removed since we don't store TW values
}

DAWG::~DAWG()
{
	delete[] delta0;
	delete[] delta1;
	delete[] d0end;
	delete[] d1end;
	delete valueDelta;
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

void DAWG::insert(VSet word, int tw)
{
	static int counter = 0;

	//Special case: if there are no transitions, we just insert the word normally
	if (delta0[0].empty() && delta1[0].empty())
	{
		insertIntoEmpty(word, tw);
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
	auto newValueDelta = new std::unordered_map<State, int>;


	//Make a product construction with our existing automaton
	std::vector<std::map<std::pair<State, State>, State>> pairMap(length + 1);
	std::vector<std::unordered_set<State>> sinkStates(length + 1);



	State initialPair = newState();
	pairMap[0][{initial, newInitial}] = initialPair;

	int totalTransitions = 0;

	//TODO zero case
	for (int layer = 0; layer < length; ++layer)
	{


		//We start our layers with the same transitions as our old delta
		newDelta1[layer] = delta1[layer];
		newDelta0[layer] = delta0[layer];

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

		//Cache a bunch of iterator ends
		auto loopEnd = thisDelta[layer].end();
		auto pairMapEnd = pairMap[layer].end();
		auto nextPairMapEnd = pairMap[layer + 1].end();
		auto newDeltaEnd = newDelta[layer].end();
		auto sinkStatesEnd = sinkStates[layer].end();


		for (auto iter = thisDelta[layer].begin(); iter != loopEnd; ++iter/*iter = thisDelta[layer].erase(iter)*/)
		{
			State qFrom = iter->first;
			State qTo = iter->second;

			//Check, are we in the pair map? If not, then we are not reachable from a previous layer
			auto searchInfo = pairMap[layer].find({ qFrom, newFrom });
			if (searchInfo != pairMapEnd)
			{
				//Generate an int for our pair state, and store it in the map
				State pairRep = searchInfo->second;

				//See if the state we're going to is in the pair map
				//And if not, create a new state for it
				State nextRep = newState();
				auto nextSearchInfo = pairMap[layer + 1].emplace(std::pair<State, State>(qTo, newTo), nextRep);
				//If we already have this vertex pair in our map, the undo our new state creation, and use that one
				if (!nextSearchInfo.second)
				{
					nextRep = nextSearchInfo.first->second;
					this->nextState--; //Undo our unneeded newstate generation
				}


				//Insert our new transition into our new store
				newDelta[layer].emplace(pairRep, nextRep);
			}

			//Check if we need to add this transition as a successor to a  "sink" transition
			if (sinkStates[layer].find(qFrom) != sinkStatesEnd)
			{
				sinkStates[layer + 1].insert(qTo);
			}
			else
			{
				newDelta[layer].erase(qFrom);
			}

		}

		//For each transition qFrom -> b -> qTo, where b doesn't match our new word
		//we send (qFrom, newFrom) to (qTo, SINK), which we represent as qTo to save labels
		//Look at all the states with transitions defined the same as our new word
		//Create new states for their pairs, and add them to the map
		loopEnd = thatDelta[layer].end();
		for (auto iter = thatDelta[layer].begin(); iter != loopEnd; ++iter/*iter = thatDelta[layer].erase(iter)*/) //TODO erase as we go?
		{
			State qFrom = iter->first;
			State qTo = iter->second;

			//Generate an int for our pair state, and store it in the map

			auto searchInfo = pairMap[layer].find({ qFrom, newFrom });
			if (searchInfo != pairMapEnd)
			{
				State pairRep = searchInfo->second;

				//First, check if we have a transition already defined on newBit
				//Try inserting our sink state, will fail if we already have a transition defined
				auto insertPairRep = newDelta[layer].emplace(pairRep, newTo);
				if (insertPairRep.second)
				{
					//If the insert succeeded, the sink state is our transition
					//Add this "sink pair" to our map, so we know to add its successors
					sinkStates[layer + 1].insert(newTo);

				}

				//Use the pair we already assigned
				pairRep = searchInfo->second;

				//Insert our new transition into our new store
				newOtherDelta[layer].emplace(pairRep, qTo);
				//Mark the sink state as seen in our pair map
				sinkStates[layer + 1].insert(qTo);
			}

			//Check if we need to add this transition as a successor to a  "sink" transition
			if (sinkStates[layer].find(qFrom) != sinkStatesEnd)
			{
				//newOtherDelta[layer][qFrom] = qTo;
				sinkStates[layer + 1].insert(qTo);
			}
			else
			{
				newOtherDelta[layer].erase(qFrom);
			}

		}

		//Look at the transition in the automaton accepting our added word
		//And add it if we added the transition before it
		if (sinkStates[layer].find(newFrom) != sinkStatesEnd)
		{
			newDelta[layer].emplace(newFrom, newTo); //Transitions from single word automaton aren't added yet
			//Mark this state as seen, so we add its sucessors
			sinkStates[layer + 1].insert(newTo);
		}


		//Save memory, we don't ever look back a level
		pairMap[layer].clear();
		sinkStates[layer].clear();

		//Helps us only minimize when we have to
		totalTransitions += newDelta[layer].size() + newOtherDelta[layer].size();


	}

	//Copy our old delta into our new one
	*newValueDelta = *valueDelta;

	//Now that we're at the last layer, add transitions to our TW states as necessary
	//This lets us store TW values in our DAWG
	auto loopEnd = valueDelta->end();
	auto lengthSinkEnd = sinkStates[length].end();
	auto pairMapLenghtEnd = pairMap[length].end();
	for (auto iter = valueDelta->begin(); iter != loopEnd; ++iter)
	{
		State newFrom = newStates[length];
		State qFrom = iter->first;
		int oldTW = iter->second;

		//Add remove unused old transitions
		if (sinkStates[length].find(qFrom) == lengthSinkEnd)
		{
			newValueDelta->erase(qFrom);
		}

		//As an automaton, our new product state can transition on either oldTW or newTW
		//But we will only ever take the min value
		auto searchInfo = pairMap[length].find({ qFrom, newFrom });
		if (searchInfo != pairMapLenghtEnd)
		{
			State pairRep = searchInfo->second;
			//we have a "sink" transition on every TW-value that was supported, 
			//plus our new TW value
			newValueDelta->emplace(pairRep, std::min(oldTW, tw));
		}

	}

	//Add a sink transition to our final state reading input tw, if necessary
	if (sinkStates[length].find(newStates[length]) != lengthSinkEnd)
	{
		newValueDelta->emplace(newStates[length], tw);
}


	//Empty our old deltas
	delete[] delta0;
	delete[] delta1;
	delete[] d0end;
	delete[] d1end;
	delete valueDelta;

	//Set our new delta values
	delta0 = newDelta0;
	delta1 = newDelta1;
	valueDelta = newValueDelta;
	setIterEnds();

	initial = initialPair;

#ifdef DEBUG
	int sizeBefore = size();
	std::string dotBefore = asDot();
#endif

	//When we're done, minimize to save space
	//TODO delete irrelevant vertices?
	if (counter % 15 == 0) //TODO make this smarter?
	{
		minimize();
	}
	counter++;

#ifdef DEBUG
	int sizeAfter = size();
	if (sizeAfter != sizeBefore)
	{
		std::cerr << "Size before " sizeBefore << " size after " << sizeAfter << "\n";
		std::cerr << "\n\n" << dotBefore << "\n\n\n" << asDot() << "\n\n";
		abort();
	}

#endif
}

void DAWG::insertIntoEmpty(VSet word, int tw)
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
	valueDelta->insert({ currentState, tw });


}

int DAWG::contains(VSet word)
{
	State currentState = initial;
	for (int i = 0; i < word.maxNumVerts; ++i)
	{
		State nextState = delta(i, currentState, word.contains(i));
		if (nextState == SINK)
		{
			return NOT_CONTAINED;
		}
		currentState = nextState;
	}
	//Return the smallest treewidth value we can follow from this state
	return (*valueDelta)[currentState];
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

std::pair<VSet, int> DAWG::nextIter()
{

	bool didFail = true;
	while (didFail)
	{
		didFail = false;
		if (iterStack.empty())
		{
			//std::cout << "Returning empty stack\n";
			VSet ret;
			return{ ret, 0 };
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
			int minTW = valueDelta->find(current)->second;
			return{ currentSet, minTW };


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
