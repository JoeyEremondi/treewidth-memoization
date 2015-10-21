#include "DAWG.hh"
#include "qset.hh"
#include <iostream>
#include <sstream>
#include <algorithm>

//#define DEBUG

std::string DAWG::asDot()
{
	std::unordered_map<State, State> labels;
	State nextLabel = 1;

	std::ostringstream ss;
	ss << "digraph G {\n";
	for (int i = 0; i < length; i++)
	{
		for (auto pair : delta0[i])
		{
			for (State q : {pair.first, pair.second})
			{
				if (labels[q] == 0)
				{
					labels[q] = nextLabel;
					nextLabel++;
				}
			}

			ss << "  " << pair.first << " -> " << pair.second << " [label=\"0 " << i << " \"]\n";
		}
		for (auto pair : delta1[i])
		{
			for (State q : {pair.first, pair.second})
			{
				if (labels[q] == 0)
				{
					labels[q] = nextLabel;
					nextLabel++;
				}
			}
			ss << "  " << pair.first << " -> " << pair.second << " [label=\"1 " << i << " \"]\n";
		}
	}

	if (labels[FINAL] == 0)
	{
		labels[FINAL] = nextLabel;
		nextLabel++;
	}

	for (auto pair : valueDelta)
	{
		//All states should have labels by now
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
	if (nextState <= ret)
	{
		std::cerr << "Overflow of states\n";
		abort();
	}
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


	if (layer == length) //Transitions are on our possible TW values
	{
		auto findVal = valueDelta.find(q);
		if (findVal != valueDelta.end())
		{
			int twVal = findVal->second;
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
		else
		{
			std::cerr << "ERROR: improperly trimmed, " << q << " not in valueDict\n";
			for (auto trans : valueDelta)
			{
				std::cerr << "Value dict trans " << trans.first << " " << trans.second << "\n";
			}
			abort();
		}

	}

	auto& sMapPlus1 = StateMap[layer + 1];
	auto sEndPlus1 = sMapPlus1.end();

	State tnext0;
	State tnext1;

	//Look at our true transition
	auto tnextIter = delta1[layer].find(q); //delta(layer, q, bit);
	//Don't follow transitions that aren't there
	if (tnextIter != delta1[layer].end())
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
	if (tnextIter != delta0[layer].end())
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
		valueDelta.erase(q);
	}
}


void DAWG::clear()
{
	for (int i = 0; i < length; i++)
	{
		delta0[i].clear();
		delta1[i].clear();
	}
	valueDelta.clear();
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
	//First layer: delete transitions to NOT_CONTAINED
	auto transitionIter = valueDelta.begin();
	auto valueEnd = valueDelta.end();
	while (transitionIter != valueEnd)
	{
		if (transitionIter->second == NOT_CONTAINED)
		{
			transitionIter = valueDelta.erase(transitionIter);
		}
		else
		{
			++transitionIter;
		}
	}

	//Second layer: delete transitions that point to something not in ValueDelta
	auto iter = delta0[length - 1].begin();
	auto loopEnd = delta0[length - 1].end();
	while (iter != loopEnd)
	{
		if (valueDelta.find(iter->second) == valueEnd)
		{
			iter = delta0[length - 1].erase(iter);
		}
		else
		{
			++iter;
		}
	}
	//Same for delta1
	iter = delta1[length - 1].begin();
	loopEnd = delta1[length - 1].end();
	while (iter != loopEnd)
	{
		if (valueDelta.find(iter->second) == valueEnd)
		{
			iter = delta1[length - 1].erase(iter);
		}
		else
		{
			++iter;
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
			auto d0nextEnd = d0next.end();//d0end[i + 1];
			auto d1next = delta1[i + 1];
			auto d1nextEnd = d1next.end();//d1end[i + 1];

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
	//Never guaranteed to have a trie after minimization
	isTrie = false;

	//std::cerr << "Transitions before minimization " << numTransitions() << "\n";
	Register = new std::unordered_map<StateSignature, State>[length + 1];
	StateMap = new std::unordered_map<State, State>[length + 1];

	//EndRegister.clear();


	EndRegisterEnd = EndRegister.end();

	minimizeHelper(0, initial);

	//Finally, adjust our initial vertex if need be
	initial = StateMap[0][initial];

	delete[] Register;
	delete[] StateMap;

	EndRegister.clear();

	//Register.clear();
	//StateMap.clear();

	//std::cerr << "Transitions after minimization " << numTransitions() << "\n";
}


DAWG::DAWG()
{
	this->length = VSet::maxNumVerts;

	//Allocate our vectors

	this->delta0.resize(length);
	this->delta1.resize(length);

	//Ensure we don't infinitely recurse
	this->stagingArea = new DAWG(NULL);

}

//Special version to avoid infinite recursion
//So we can store a DAWG inside a DAWG
//TODO special Trie class?
DAWG::DAWG(DAWG* staging)
{
	this->length = VSet::maxNumVerts;

	//Allocate our vectors

	this->delta0.resize(length);
	this->delta1.resize(length);

	this->stagingArea = staging;
	this->stagingInitial = SINK;

}

void DAWG::copyFrom(const DAWG & that)
{
	//Initialize length and pointers
	this->length = that.length;

	//Copy delta0 and delta1 from input
	for (int i = 0; i < length; i++)
	{
		this->delta0[i] = that.delta0[i];
		this->delta1[i] = that.delta1[i];
	}

	//Point each entry to NO_WIDTH in ValueDelta to show that they're from an old level
	for (auto transition : that.valueDelta)
	{
		valueDelta.emplace(transition.first, NOT_CONTAINED);
	}



	//Set our initial and next state values
	this->initial = that.initial;
	this->nextState = that.nextState;

	minimize(); //There could be lots of redundancy removed since we don't store TW values
}

DAWG::~DAWG()
{
	if (stagingArea != NULL)
	{
		delete stagingArea;
	}

	delta0.clear();
	delta1.clear();
	valueDelta.clear();
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
	v.clear();
	if (q == SINK)
	{
	}
	else if (depth == length)
	{
		std::ostringstream ss;
		auto searchInfo = valueDelta.find(q);
		if (searchInfo != valueDelta.end())
		{
			ss << "(" << searchInfo->second << ")";
			v.push_back(ss.str());
		}

	}
	else
	{
		auto sub1 = wordSetHelper(depth + 1, delta(depth, q, true));
		if (!sub1.empty())
		{
			for (auto subWord = sub1.begin(); subWord != sub1.end(); ++subWord)
			{
				v.push_back("1" + *subWord);
			}
		}

		auto sub2 = wordSetHelper(depth + 1, delta(depth, q, false));
		if (!sub2.empty())
		{
			for (auto subWord : sub2)
			{
				v.push_back("0" + subWord);
			}
		}


	}
	return v;
}

void DAWG::unionWithStaging()
{


	//auto newDelta0 = new std::unordered_map<State, State>[length];
	//auto newDelta1 = new std::unordered_map<State, State>[length];
	//auto newValueDelta = new std::unordered_map<State, int>;


	//Make a product construction with our existing automaton
	std::vector<std::map<std::pair<State, State>, State>> pairMap(length + 1);
	std::vector<std::unordered_set<State>> sinkStates(length + 1);
	std::vector<std::unordered_set<State>> stagingSinks(length + 1);


	State initialPair = newState();
	pairMap[0][{initial, stagingInitial}] = initialPair;

	int totalTransitions = 0;

	//TODO zero case
	for (int layer = 0; layer < length; ++layer)
	{


		//State newFrom = newStates[layer];
		//State newTo = newStates[layer + 1];
		//bool newBit = word.contains(layer);

		//For each transition matching the letter of our new automaton
		//we add a transition to a new "pair" state
		for (bool newBit : {false, true})
		{
			auto& thisDelta = newBit ? delta1 : delta0;
			auto& thatDelta = newBit ? delta0 : delta1;
			auto& stagingDelta = newBit ? stagingArea->delta1 : stagingArea->delta0;
			auto& stagingOtherDelta = newBit ? stagingArea->delta0 : stagingArea->delta1;

			std::unordered_map<State, State> newDelta;
			std::unordered_map<State, State> newOtherDelta;
			//std::unordered_set<State> eraseList;

			//Look at all the states with transitions defined the same as our new word
			//Create new states for their pairs, and add them to the map

			//Cache a bunch of iterator ends
			auto loopEnd = thisDelta[layer].end();
			auto pairMapEnd = pairMap[layer].end();
			auto nextPairMapEnd = pairMap[layer + 1].end();
			auto sinkStatesEnd = sinkStates[layer].end();

			auto transIter = thisDelta[layer].begin();
			auto deltaEnd = thisDelta[layer].end();
			auto stagingEnd = stagingDelta[layer].end();

			//Look at each pair of states in the layers we're combining
			while (transIter != deltaEnd)
			{
				State qFrom = transIter->first;
				State qTo = transIter->second;

				auto stagingIter = stagingDelta[layer].begin();
				while (stagingIter != stagingEnd)
				{
					State newFrom = stagingIter->first;
					State newTo = stagingIter->second;

					auto pairIter = pairMap[layer].find({ qFrom, newFrom });
					if (pairIter != pairMapEnd)
					{
						//Generate an int for our pair state, and store it in the map
						State pairRep = pairIter->second;

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
						newDelta.emplace(pairRep, nextRep);
					}




					//TODO delte if need be
				}

				//After we'd gone through all matches, add edges to sink-state pairs
				for (auto otherPair : stagingOtherDelta[layer])
				{

					State newFrom = otherPair.first;
					State newTo = otherPair.second;
					auto pairIter = pairMap[length].find({ qFrom, newFrom });
					//Only do this if the pair is reachable
					if (pairIter != pairMapEnd)
					{
						State pairState = pairIter->second;
						//TODO single intersection?
						//First, check if for (q,r), if transition on b is defined for r
						auto falseInfo = stagingDelta[layer].find(newFrom);
						if (falseInfo == stagingDelta[layer].end())  //TODO cache iter?
						{
							newDelta.emplace(pairState, qTo);
							sinkStates[layer].insert({ qTo, SINK });
						}

						//Then, check if !b transition defined for q
						auto trueInfo = thisDelta[layer].find(qTo);
						if (trueInfo == thisDelta[layer].end())
						{
							newOtherDelta.emplace(pairState, newTo);
							sinkStates[layer].insert({ SINK, newTo });
						}
					}
				}



				//Check if we need to add this transition as a successor to a  "sink" transition, and keep it in our delta
				//Or if we delete it
				if (sinkStates[layer].find(qFrom) != sinkStatesEnd)
				{
					sinkStates[layer + 1].insert(qTo);
					++transIter;
				}
				else
				{
					transIter = thisDelta[layer].erase(transIter);

				}


			}

			//Add all our transitions we generated in our first traversal
			for (auto newTrans : newDelta)
			{
				thisDelta[layer].insert(newTrans);
			}

			//add transitions from staging area into this layer, if we reached them in sinkStates in past layer
			for (auto stagedTrans : stagingDelta[layer])
			{
				State newFrom = stagedTrans.first;
				State newTo = stagedTrans.second;
				auto searchInfo = stagingSinks[layer].find(newFrom);
				if (searchInfo != stagingSinks[layer].end())
				{
					thisDelta[layer].emplace(newFrom, newTo);
					stagingSinks[layer + 1].emplace(newTo);
				}
			}
		}

	}

	//Merge our final deltas
	
	std::unordered_map<State, int> newValue;
	for (auto pairPair : pairMap[length])
	{
		State qFrom = pairPair.first.first;
		State newFrom = pairPair.first.second;
		//TODO assume can reach a value delta?
		int newTW = std::min(valueDelta[qFrom], stagingArea->valueDelta[newFrom]);
		newValue.emplace(pairPair.second, newTW);
	}
	for (auto q : sinkStates[length])
	{
		newValue.emplace(q, valueDelta[q]);
	}
	for (auto q : stagingSinks[length])
	{
		newValue.emplace(q, stagingArea->valueDelta[q]);
	}
	//Replace our valueDelta;
	valueDelta.clear();
	valueDelta = newValue;


	//Now that we're done, set our new initial state;
	this->initial = initialPair;

	//And, minimize our combined automaton
	this->minimize();

	//Delete and re-initialize our staging area
	this->stagingArea->clear();
	delete this->stagingArea;
	this->stagingArea = new DAWG(NULL);
}

void DAWG::insert(VSet word, int tw)
{
	this->stagingArea->insertIntoEmpty(word, tw);
	if (this->stagingArea->numTransitions() > maxTransitions)
	{
		this->stagingArea->minimize();
		this->unionWithStaging();
		this->maxTransitions = ABS_MAX_TRANSITIONS - this->numTransitions();
		if (maxTransitions < 1)
		{
			std::cerr << "Ran out of transitions\n";
			abort();
		}
	}
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
	valueDelta.insert({ currentState, tw });


}

int DAWG::contains(VSet word)
{
	//First, check if it's in our staging area
	if (stagingArea != NULL)
	{
		int stageContains = stagingArea->contains(word);
		if (stageContains != NOT_CONTAINED)
		{
			return stageContains;
		}
	}

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
	auto searchInfo = valueDelta.find(currentState);
	if (searchInfo == valueDelta.end())
	{
		return NOT_CONTAINED;
	}
	return searchInfo->second;
}

DAWG::iterator DAWG::begin()
{
	//Make sure we've merged with our staging area before we iterate
	this->stagingArea->minimize();
	this->unionWithStaging();
	this->minimize();
	return DAWG::iterator({ initial, 0, VSet(), std::vector<State>() }, this);
}

DAWG::iterator DAWG::end()
{
	return iterator();
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



bool DAWG::empty()
{
	return delta(0, initial, false) == SINK && delta(0, initial, true) == SINK;
}

DAWG::iterator::iterator(StackElem elem, DAWG * super)
	: iterStack()
{
	iterStack.push_back(elem);
	this->super = super;
}

DAWG::iterator DAWG::iterator::nextIter()
{
	//Don't let us advance past the end
	if (iterStack.empty() && super == NULL)
	{
		return *this;
	}
	bool didFail = true;
	while (didFail)
	{
		didFail = false;
		if (iterStack.empty())
		{
			//std::cout << "Returning empty stack\n";
			VSet ret;
			currentPair = { ret, 0 };
			return *this;
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

		while (currentPos < super->length)
		{
			State q0, q1;
			if ((q0 = super->delta(currentPos, current, false)) != super->SINK && (q1 = super->delta(currentPos, current, true)) != super->SINK)
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
			else if ((q0 = super->delta(currentPos, current, false)) != super->SINK)
			{
				//std::cout << "Following " << current << " -> false -> " << q0 << " layer " << currentPos + 1 << "\n";

				current = q0;
				currentSet.erase(currentPos);
				currentPos++;
				currentPath.push_back(q0);

			}
			else if ((q1 = super->delta(currentPos, current, true)) != super->SINK)
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
			auto searchInfo = super->valueDelta.find(current);
			if (searchInfo == super->valueDelta.end())
			{
				std::cerr << "ERROR: Found end-state with no TW value";
				abort();
			}
			else if (searchInfo->second == super->NOT_CONTAINED)
			{
				std::cerr << "ERROR: iterate found non-contained set " << showSet(currentSet) << " " << searchInfo->second << "\n";
				std::cerr << "State path ";
				for (State st : currentPath)
				{
					std::cerr << st << " ";
				}
				std::cerr << "\n";
				for (auto pair : super->valueDelta)
				{
					std::cerr << "Final trans " << pair.first << " " << pair.second << "\n";
				}
				abort();
			}
			this->currentPair = { currentSet, searchInfo->second };



		}

	}
	return *this;
}
