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
    std::vector<Vertex> orderVertices(std::set<Vertex> S) ;
    int upperBound(std::set<Vertex> S) ;
    int lowerBound(std::set<Vertex> S) ;
    bool shouldCache(std::set<Vertex> S) ;
    bool needsCleaning() ;
    std::vector<std::set<Vertex>> setsToForget(std::set<Vertex> S) ;
};

#endif   
