#ifndef __BASIC_MEMO__H_
#define __BASIC_MEMO__H_

#include "AbstractMemo.hh"
#include "qset.hh"

#include <map>

class BasicMemo : public AbstractMemo
{
    //No no methods, we just implement the virtual ones from AbstractMemo
    //We can use the default constructor and destructor
protected:
    std::vector<Vertex> orderVertices(VSet S) ;
    int upperBound(VSet S) ;
    int lowerBound(VSet S) ;
    bool shouldCache(VSet S) ;
    bool needsCleaning() ;
    std::vector<VSet> setsToForget(VSet S) ;
public:
    BasicMemo(Graph G);
    ~BasicMemo();
};

#endif   
