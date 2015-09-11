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
	void orderVertices(std::vector<Vertex>& vec);
	int upperBound(const VSet& S);
	int lowerBound(const VSet& S);
	bool shouldCache(const VSet& S);
	bool needsCleaning();
	std::vector<VSet> setsToForget(const VSet& S);
public:
	BasicMemo(Graph G);
	~BasicMemo();
};

#endif   
