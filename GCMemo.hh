#ifndef __GC_MEMO__H_
#define __GC_MEMO__H_

#include "memo.hh"
#include "qset.hh"
#include "DepthBoundedMemo.hh"


/**
An abstract class for "Garbage collecting" (GC) memoizing:
The maximum number of values which can be memoized
is given as a parameter.

When a new value is to be stored, but we have reached our limit, 
some elements are deleted from our set.

The choice of which elements to delete is determined by the definition
of "elemsToForget", which must be provided in instantiations
of this abstract class.
*/
class GCMemo : public DepthBoundedMemo
{
private:
    unsigned long maxElems;
    
    
public:
    GCMemo(unsigned long bound, unsigned long maxElemsParam);
    ~GCMemo();
    int subTW(std::set<Vertex> S, Graph G);
    virtual std::set<std::set<Vertex> > elemsToRemove() = 0;
    //Called each time subTW is called, to store information about 
    //Which sets have been used so far, are likely to be used again, etc.
    virtual void recordMemoInfo(std::set<Vertex>) = 0;
    
};

#endif 
