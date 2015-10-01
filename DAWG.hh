#pragma once

#include "VSet.hh"

typedef int State;

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
	int numStates;
	State initial;
	std::map<State, State> delta0; //TODO array for space?
	std::map<State, State> delta1;
	const State SINK = -1;
	std::vector<StackElem> iterStack;
	StackElem currentStack;

protected:
	State newState();
	void addTransition(int from, int to, bool readLetter);
	void minimize();

public:
	void insert(VSet word);
	bool contains(VSet word);
	inline State delta(State q, bool bitRead) { return bitRead ? delta1[q] : delta0[q]; }
	void initIter();
	VSet nextIter();
	bool iterDone();
	bool empty();
};
