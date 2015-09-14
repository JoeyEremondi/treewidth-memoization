#ifndef __BOTTOM__UP__TW__H__
#define __BOTTOM__UP__TW__H__

#include "qset.hh"
#include "TWUtilAlgos.hh"
#include "AbstractMemo.hh"

int bottomUpTW(Graph G);

int bottomUpTWFromSet(VSet clique, const Graph& G, const VSet& SStart, int upperBound);

int calcUpperBound(Graph G, VSet S);




#endif
