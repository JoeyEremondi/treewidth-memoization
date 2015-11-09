#pragma once

#include "VSet.hh"

/**
This class contains an abstract implementation of the bottom-up algorithm from
"Exact algorithms for treewidth".
It allows us to compare different implementations of
*/
class AbstractBottomUp
{
public:
	/*Create a new instance of the solver for a given graph*/
	AbstractBottomUp(const Graph& G);
	
	/*Standard destructor*/
	~AbstractBottomUp();
	
	/*Run the algorithm and return the treewidth of the graph given to the constructor*/
	int tw();
	
protected:
	/*Used internally: the when we iterate through the subsets and treewidth values
	from the previouis iteration of the algorithm, we store their values in S and r respectively*/
	VSet S;
	int r;
	
	/*Store the graph from our construcor*/
	const Graph& G;
	/*Cache the size of the graph*/
	int nGraph;

	/*We continually refine the upper-bound on the final tw value
	as we run our algorithm.*/
	int globalUpperBound;

	/*Store our current iteration so we can get at it from various methods*/
	int currentLayer = 0;
	
	/*The global lower bound on tw that we've computed
	Not currently used*/
	int lowerBound;

	/*Use the bottom-up method to find the TW value for the set S.
	Takes a set as input so that we can eliminate vertices from the max-clique of the graph.*/
	int twForSet(VSet S);
	
	/////////// Abstract Methods //////////////

	/*Initialize iterating through the stored values from the last iteration of the algorithm.
	Sets S and r to the first values iterated through.*/
	virtual void beginIter() = 0;
	
	/*Set S and r to the next values in our itertaion order,
	if any such values exist*/
	virtual void iterNext() = 0;

	/*Returns true if we've already iterated through all values in the previous layer
	since the last time we called beginIter(), false otherwise*/
	virtual bool iterDone() = 0;
	
	/*Given a potential (possibly too large) value tw for a set S,
	
	*/
	virtual void updateTW(int layer, VSet S, int tw) = 0;

	/*Called before begin an iteration of the bottom-up algorithm.
	Lets us initialize any data-structures we need.*/
	virtual void beginLayer(int layer) = 0;
	/*Called after we finish each iteration of the bottom-up algorithm.
	Lets us clean up anything we need after each layer.*/
	virtual void endLayer(int layer) = 0;

	/*Extract the final TW value from our previous computations.*/
	virtual int finalResult(int finalLayer, VSet SStart) = 0;

	/*Used for memory-management: returns the number of (S,r) pairs we
	currently have stored.*/
	virtual int numStored() = 0;
};

