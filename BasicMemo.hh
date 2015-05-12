#ifndef __BASIC_MEMO__H_
#define __BASIC_MEMO__H_

#include "memo.hh"
#include "qset.hh"

#include <map>

class BasicMemo : public Memoizer
{
protected:
    Graph g;    
    std::map<std::set<Vertex>,int>* storedCalls;
    
    
public:
    BasicMemo();
    ~BasicMemo();
    int subTW(std::set<Vertex> S, Graph G);
    std::vector<Vertex> orderVertices(std::set<Vertex> S, Graph G);
    
};

#endif   
