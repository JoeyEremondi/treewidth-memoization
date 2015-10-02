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
	if (depth == length - 1)
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
