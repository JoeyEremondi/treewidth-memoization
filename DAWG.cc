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
	auto dotBefore = this->asDot();
	auto stageDot = stagingArea->asDot();


	//Make a product construction with our existing automaton
	std::vector<std::map<std::pair<State, State>, State>> pairMap(length + 1);

	State initialPair = newState();
	pairMap[0][{initial, stagingArea->initial}] = initialPair;

	int totalTransitions = 0;

	//TODO zero case
	for (int layer = 0; layer < length; ++layer)
	{


		//State newFrom = newStates[layer];
		//State newTo = newStates[layer + 1];
		//bool newBit = word.contains(layer);
		std::unordered_map<State, State> newDelta0;
		std::unordered_map<State, State> newDelta1;

		for (auto pairPair : pairMap[layer])
		{
			State qFrom = pairPair.first.first;
			State newFrom = pairPair.first.second;
			State pairRep = pairPair.second;

			//std::cout << "Looking at pair " << qFrom << " " << newFrom << "\n";

			auto searchMain0 = delta0[layer].find(qFrom);
			bool main0 = searchMain0 != delta0[layer].end();
			auto searchMain1 = delta1[layer].find(qFrom);
			bool main1 = searchMain1 != delta1[layer].end();

			auto searchStaging0 = stagingArea->delta0[layer].find(newFrom);
			bool stage0 = searchStaging0 != stagingArea->delta0[layer].end();
			auto searchStaging1 = stagingArea->delta1[layer].find(newFrom);
			bool stage1 = searchStaging1 != stagingArea->delta1[layer].end();

			//Special cases: if our state is a "sink" state in the copy of one of our automata
			//Just copy transitions either from our old machine, or the staging area
			if (qFrom == SINK)
			{
				//Add 0 transition
				if (stage0)
				{
					State newTo = searchStaging0->second;
					newDelta0.emplace(newFrom, newTo);
					pairMap[layer + 1].insert({ { SINK, newTo }, newTo });
				}

				//Add 1 transition
				if (stage1)
				{
					State newTo = searchStaging1->second;
					newDelta1.emplace(newFrom, newTo);
					pairMap[layer + 1].insert({ { SINK, newTo }, newTo });
				}
			}
			else if (newFrom == SINK)
			{
				//Add 0 transition
				if (main0)
				{
					State qTo = searchMain0->second;
					newDelta0.emplace(qFrom, qTo);
					pairMap[layer + 1].insert({ { SINK, qTo }, qTo });
				}

				//add 1 transition
				if (main1)
				{
					State qTo = searchMain1->second;
					newDelta1.emplace(qFrom, qTo);
					pairMap[layer + 1].insert({ { SINK, qTo }, qTo });
				}
			}
			else
			{
				//Add 0 transition
				//3 cases: either defined for both automata, or for just one
				//Do nothing if defined for neither
				if (stage0 && main0)
				{
					std::cout << "Case1\n";
					State qTo = searchMain0->second;
					State newTo = searchStaging0->second;
					State pairTo = newState();
					pairMap[layer + 1].insert({ {qTo, newTo}, pairTo });
					newDelta0.emplace(pairRep, pairTo);
				}
				else if (stage0 && !main0)
				{
					std::cout << "Case2\n";
					//add transition from pairRep to staging area automaton
					State newTo = searchStaging0->second;
					pairMap[layer + 1].insert({ { SINK, newTo }, newTo });
					newDelta0.emplace(pairRep, newTo);
				}
				else if (!stage0 && main0)
				{
					std::cout << "Case3\n";
					//add transition from pairRep to copy of main automaton
					State qTo = searchMain0->second;
					pairMap[layer + 1].insert({ { qTo, SINK}, qTo });
					newDelta0.emplace(pairRep, qTo);
				}

				//Add 1 transition
				//3 cases: either defined for both automata, or for just one
				//Do nothing if defined for neither
				if (stage1 && main1)
				{
					std::cout << "Case4\n";
					State qTo = searchMain1->second;
					State newTo = searchStaging1->second;
					State pairTo = newState();
					pairMap[layer + 1].insert({ { qTo, newTo }, pairTo });
					newDelta1.emplace(pairRep, pairTo);
				}
				else if (stage1 && !main1)
				{
					std::cout << "Case5\n";
					//add transition from pairRep to staging area automaton
					State newTo = searchStaging1->second;
					pairMap[layer + 1].insert({ { SINK, newTo }, newTo });
					newDelta1.emplace(pairRep, newTo);
				}
				else if (!stage1 && main1)
				{
					std::cout << "Case6\n";
					//add transition from pairRep to copy of main automaton
					State qTo = searchMain1->second;
					pairMap[layer + 1].insert({ { qTo, SINK }, qTo });
					newDelta1.emplace(pairRep, qTo);
				}
			}

		}
		//Replace our delta0 and delta1 with our new ones
		this->delta0[layer].clear();
		this->delta0[layer].insert(newDelta0.begin(), newDelta0.end());
		this->delta1[layer].clear();
		this->delta1[layer].insert(newDelta1.begin(), newDelta1.end());

	}

	//Merge our final deltas

	std::unordered_map<State, int> newValue;
	for (auto pairPair : pairMap[length])
	{
		State qFrom = pairPair.first.first;
		State newFrom = pairPair.first.second;

		if (qFrom == SINK)
		{
			newValue.emplace(newFrom, stagingArea->valueDelta[newFrom]);
		}
		else if (newFrom == SINK)
		{
			newValue.emplace(qFrom, valueDelta[qFrom]);
		}
		else
		{
			State pairRep = pairPair.second;
			int newTW = std::min(valueDelta[qFrom], stagingArea->valueDelta[newFrom]);
			newValue.emplace(pairPair.second, newTW);
		}

	}
	//Replace our valueDelta;
	valueDelta.clear();
	valueDelta.insert(newValue.begin(), newValue.end());


	//Now that we're done, set our new initial state;
	this->initial = initialPair;

	//And, minimize our combined automaton
	this->minimize();

	//Delete and re-initialize our staging area
	this->stagingArea->clear();
	delete this->stagingArea;
	this->stagingArea = new DAWG(NULL);

	std::cout << "Main area before\n" << dotBefore << "\n";
	std::cout << "\n\nStaging before\n" << stageDot << "\n";
	std::cout << "\nFinal aut\n" << asDot() << "\n";


}

void DAWG::insert(VSet word, int tw)
{
	if (this->stagingArea == NULL)
	{
		this->insertIntoEmpty(word, tw);
		return;
	}
	this->stagingArea->insert(word, tw);
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
	this->stagingArea->minimize();

	//Make sure we've merged with our staging area before we iterate
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
