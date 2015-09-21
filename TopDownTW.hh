#ifndef __TOP__DOWN__TW__H__
#define __TOP__DOWN__TW__H__

#include "qset.hh"
#include "TWUtilAlgos.hh"
#include "AbstractMemo.hh"
#include "BottomUpTW.hh"

class TopDownTW
{
public:
	TopDownTW(const Graph& G);
	int topDownTW(const Graph& G);

	int topDownTWFromSet(const Graph& G, const VSet& S, int nSet);

private:
	const Graph& G;
	int sharedUpperBound;
	std::vector<Vertex> allVertices;
	BottomUpTW bottomUpInfo;
};



#endif
