#ifndef __TOP__DOWN__TW__H__
#define __TOP__DOWN__TW__H__

#include "qset.hh"
#include "TWUtilAlgos.hh"
#include "AbstractMemo.hh"

int topDownTW(const Graph& G);

int topDownTWFromSet(const Graph& G, const VSet& SStart, int globalUpperBound);

#endif
