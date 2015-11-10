#pragma once

#include "qset.hh"
#include "TWUtilAlgos.hh"
#include "BottomUpTW.hh"

/* An abstract class, contains the top-down algorithm,
and abstracts over the storage methods for caching and forgetting values*/
class AbstractTopDown
{


public:
	/*Make a new instance of the algorithm runner,
	to find the treewidth of the given graph*/
	AbstractTopDown(const Graph& G, int maxBottom);

	/*Run the algorithm and get the calculated treewidth*/
	int topDownTW();

	/*Get the value stored for the maximum bottom-up size*/
	int getMaxBottomUp();

protected:
	//How many transitions we allow our bottom-up solver to have before aborting
	int maxBottomUpSize = 1;

	//The global upper and lower bounds, refined as we go
	int sharedUpperBound;
	int lowerBound;

	//Store the graph, and pre-compute its size and vertex vector
	const Graph& G;
	int nGraph;
	std::vector<Vertex> allVertices;

	//Store the instance of our bottom-up solver for the hybrid approach
	BottomUpTW bottomUpInfo;

	//Recursive method to calculate the TW value for the given set
	//We give its size for efficiency
	int topDownTWFromSet(const Graph& G, const VSet& S, int nSet);

	
	/////////// Abstract methods ////////////////

	/*Returns whether the current set with the given size
	is stored in our memoized value set*/
	virtual bool isMemoized(int nSet, VSet S) = 0;
	
	/*Return the stored value for a given set with the given size.
	Should only be called if isMemoized returns true*/
	virtual int memoizedValue(int nSet, VSet S) = 0;
	
	/*Once we've calculated a TW value for a set S, give it to our store
	to cache*/
	virtual void memoizeTW(int nSet, VSet S, int tw) = 0;
	
	/*If we run out of memory, run some procedure to try to clear memory
	or forget values*/
	virtual void cleanMemoized() = 0;

	


};
