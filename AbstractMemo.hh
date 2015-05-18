#ifndef __ABSTRACT_MEMO__H_
#define __ABSTRACT_MEMO__H_

#include "memo.hh"
#include "qset.hh"
#include "BasicMemo.hh"
#include "NaiveMemo.hh"

#include <map>

//Treewidth is always positive, so we can use this in place of -Inf
const int NO_WIDTH = -1;


/**
A basic memoized treewidth search algorithm, but uses the naive (no memoization)
version when the sets are smaller than the parameter given.
*/
class AbstractMemo
{
protected:
    unsigned long bruteForceSize;
    Graph G;
    int subTW(std::set<Vertex> S);
    std::map<std::set<Vertex>,int>* storedCalls;
    int fetchOrStore(std::set<Vertex> S);
    int naiveTW(AbstractMemo* memo, std::set<Vertex> S, Graph G);

    
    virtual std::vector<Vertex> orderVertices(std::set<Vertex> S) = 0;
    virtual int upperBound(std::set<Vertex> S) = 0;
    virtual int lowerBound(std::set<Vertex> S) = 0;
    virtual bool shouldCache(std::set<Vertex> S) = 0;
    virtual bool needsCleaning() = 0;
    virtual std::vector<std::set<Vertex>> setsToForget(std::set<Vertex> S) = 0;
    
    
public:
    AbstractMemo(Graph G);
    ~AbstractMemo();
    //int subTW(std::set<Vertex> S, Graph G);

    int treeWidth();

    
    
};

#endif   
