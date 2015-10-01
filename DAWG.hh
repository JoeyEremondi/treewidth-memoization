#pragma once

#include "VSet.hh"

typedef uint64_t State;

struct StackElem
{
	State state;
	int depth;
	bool bit;
	VSet set;
};

class DAWG
{
private:
	uint64_t nextState = 2;
	const State initial = 1;
	std::vector<std::map<State, State>> delta0; //TODO array for space?
	std::vector<std::map<State, State>> delta1;
	const State SINK = 0;
	std::vector<StackElem> iterStack;
	StackElem currentStack;
	int length;

protected:
	State newState();
	void addTransition(int depth, State from, State to, bool readLetter);
	void minimize();

public:
	DAWG();
	void insert(VSet word);
	bool contains(VSet word);
	inline State delta(int depth, State q, bool bitRead) {
		if (bitRead)
		{
			auto searchInfo = delta1[depth].find(q);
			if (searchInfo == delta1[depth].end())
			{
				return SINK;
			}
			return searchInfo->second;
		}
		else
		{
			auto searchInfo = delta0[depth].find(q);
			if (searchInfo == delta0[depth].end())
			{
				return SINK;
			}
			return searchInfo->second;
		}
	}
	void initIter();
	VSet nextIter();
	bool iterDone();
	bool empty();
};
