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
    int subTW(std::set<Vertex> S);
    std::map<std::set<Vertex>,int>* storedCalls;
    int fetchOrStore(std::set<Vertex> S);
    int naiveTW(AbstractMemo* memo, std::set<Vertex> S, Graph G);

    //These should be called by any constuctors or destructors
    AbstractMemo(Graph G);
    ~AbstractMemo();
    
    virtual std::vector<Vertex> orderVertices(std::set<Vertex> S) = 0;
    virtual int upperBound(std::set<Vertex> S) = 0;
    virtual int lowerBound(std::set<Vertex> S) = 0;
    virtual bool shouldCache(std::set<Vertex> S) = 0;
    virtual bool needsCleaning() = 0;
    virtual std::vector<std::set<Vertex>> setsToForget(std::set<Vertex> S) = 0;

    
    
    
public:
    int treeWidth();

    
    
};

#endif   
