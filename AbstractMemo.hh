#ifndef __ABSTRACT_MEMO__H_
#define __ABSTRACT_MEMO__H_

#include "qset.hh"
#include <map>

//Treewidth is always positive, so we can use this in place of -Inf
const int NO_WIDTH = -1;


/**
Abstract implementation of the memoized recursive TW algorithm.
This lets us tune our heuristics without changing the core algorithm.
*/
class AbstractMemo
{
protected:
    Graph G;
    Graph GC;
    
    int numExpanded = 0;
    int memoMisses = 0;
    int memoHits = 0;

    int globalUpperBound;
    int numVerts;

    int recursionDepth = 0;
    
    

    int subTW(int lowerBound, VSet S);
    std::map<VSet,int>* storedCalls;
    int fetchOrStore(int lowerBound, VSet S);
    int naiveTW(AbstractMemo* memo, int ourLB, VSet S, Graph G);

    //These should be called by any constuctors or destructors
    AbstractMemo(Graph G);
    ~AbstractMemo();
    
    virtual std::vector<Vertex> orderVertices(VSet S) = 0;
    virtual int upperBound(VSet S) = 0;
    virtual int lowerBound(VSet S) = 0;
    virtual bool shouldCache(VSet S) = 0;
    virtual bool needsCleaning() = 0;
    virtual std::vector<VSet> setsToForget(VSet S) = 0;

    
    
    
public:
    int treeWidth();
    void printStats();
    
    
    
};

#endif   
