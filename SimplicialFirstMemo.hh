#ifndef __SIMP_FIRST_MEMO__H_
#define __SIMP_FIRST_MEMO__H_

#include "AbstractMemo.hh"
#include "BasicMemo.hh"
#include "qset.hh"


class SimplicialFirstMemo : public BasicMemo
{
    //We have an integer for the size of the smallest sets we bother caching
protected:
    //We can just keep these as global to the class:
    //When we have a simplicial vertex, we will only ever expand one of them
    //Likewise if a vertex has a single edge
    bool currentHasSimpOrSingle = false;
    Vertex currentSpecialVertex;

    //We ensure we only expand the special vertices by inflating
    //the upper bound for all other vertices
    int lowerBound(const VSet& S) ;

    //We inject the simplicial and single-vertex checks into this function
    int subTW(int lowerBound, int upperBound, const VSet& S);

    //Used to check if a vertex is simplicial in G[S]
    bool isSimplicial(Vertex V, const VSet& S);
    
    //Check if a vertex has exactly one neighbour in G[S]
    bool hasOneNeighbour(Vertex V, const VSet& S);
    
public:
    SimplicialFirstMemo(Graph G);
    ~SimplicialFirstMemo();
    
};

#endif   
