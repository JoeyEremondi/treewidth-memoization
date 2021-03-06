#ifndef __ABSTRACT_MEMO__H_
#define __ABSTRACT_MEMO__H_

#include "qset.hh"
#include <map>
#include <unordered_map>

//Treewidth is always positive, so we can use this in place of -Inf
const int NO_WIDTH = -1;


/**
Abstract implementation of the memoized recursive TW algorithm.
This lets us tune our heuristics without changing the core algorithm.
*/
class AbstractMemo
{
private:
	void setGlobalUpperBound(VSet S);
	void setGlobalLowerBound(VSet S);

protected:
	Graph G;
	Graph GC;
	int nGraph;
	bool** adjMatrix;

	int numExpanded = 0;
	int memoMisses = 0;
	int memoHits = 0;

	int numVerts;

	int recursionDepth = 0;

	int globalUpperBound;
	int globalLowerBound;

	VSet maxClique;

	//std::map<VSet,int>* storedCalls;
	std::unordered_map<VSet, int>* storedCalls;
	int fetchOrStore(int lowerBound, int upperBound, const VSet& S);
	int naiveTW(int ourLB, int ourUB, const VSet& S);

	//These should be called by any constuctors or destructors
	AbstractMemo(Graph G);
	~AbstractMemo();

	//TODO too slow as virtual?
	virtual int subTW(int lowerBound, int upperBound, const VSet& S);

	virtual void orderVertices(std::vector<Vertex>& vec) = 0;
	virtual int upperBound(const VSet& S) = 0;
	virtual int lowerBound(const VSet& S) = 0;
	virtual bool shouldCache(const VSet& S) = 0;
	virtual bool needsCleaning() = 0;
	virtual std::vector<VSet> setsToForget(const VSet& S) = 0;




public:
	int treeWidth();
	void printStats();



};

#endif   
