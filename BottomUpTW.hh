#ifndef __BOTTOM__UP__TW__H__
#define __BOTTOM__UP__TW__H__

#include "qset.hh"
#include "TWUtilAlgos.hh"
#include "AbstractMemo.hh"

int bottomUpTW(const Graph& G);

int bottomUpTWFromSet(const Graph& G, const VSet& SStart, int globalUpperBound);

int calcUpperBound(const Graph& G, const VSet& S);




#endif
