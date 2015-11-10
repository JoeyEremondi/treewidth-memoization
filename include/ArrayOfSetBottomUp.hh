#pragma once
#include "AbstractBottomUp.hh"
#include "VSet.hh"
#include "qset.hh"

//Instance of the bottom-up algorithm, where we store pair (S, r)
//as TW[r] = S
//i.e. we insert S into the rth set in our array
class ArrayOfSetBottomUp :
	public AbstractBottomUp
{
public:
	ArrayOfSetBottomUp(const Graph& G);
	~ArrayOfSetBottomUp();

protected:
	//The array to store our TW values
	std::set<VSet>** TWarr;
	//The current iterator of the set we're looking at
	std::set<VSet>::iterator iter;
	//The array index of the set we're currently looking at
	int iterTWVal;
	//Cache the end-iterator of the current set we're looking at
	std::set<VSet>::const_iterator layerEnd;
	//Keep track of this, our iteration has a special case for the first element
	bool haveSeenInitialElement = false;

	//Array of upper-bounds for each layer, acts as an "end-point" for our TW array
	//So that we don't need to iterate through the whole array
	int* upperBoundForLayer;


	/////// Overridden methods  ///////

	int TW(int layer, VSet S);
	void updateTW(int layer, VSet S, int tw);
	void beginLayer(int layer);
	void endLayer(int layer);
	int finalResult(int finalLayer, VSet SStart);
	int numStored();
	void beginIter();
	bool iterDone();
	void iterNext();
};

