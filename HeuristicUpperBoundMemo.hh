#ifndef __HUB_MEMO__H_
#define __HUB_MEMO__H_

#include "AbstractMemo.hh"
#include "BasicMemo.hh"
#include "qset.hh"

class HeuristicUpperBoundMemo : public BasicMemo
{
    //We have an integer for the size of the smallest sets we bother caching
protected:
    int bound;
    int lowerBound(VSet S) ;
    int upperBound(VSet S) ;
public:
    HeuristicUpperBoundMemo(Graph G);
    ~HeuristicUpperBoundMemo();
    
};

#endif   

