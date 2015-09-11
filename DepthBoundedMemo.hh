#ifndef __DEPTH_BOUND_MEMO__H_
#define __DEPTH_BOUND_MEMO__H_

#include "AbstractMemo.hh"
#include "BasicMemo.hh"
#include "qset.hh"

class DepthBoundedMemo : public BasicMemo
{
	//We have an integer for the size of the smallest sets we bother caching
protected:
	int bound;

	bool shouldCache(std::set<Vertex> S);
public:
	DepthBoundedMemo(int boundParam, Graph G);
	~DepthBoundedMemo();

};

#endif   

