#pragma once

#include "qset.hh"
#include "TWUtilAlgos.hh"
#include "BottomUpTW.hh"

//Basic test to determine if the neighbours in S of a vertex form a clique
bool isSimplicial(const Graph& G, Vertex v, const VSet& SStart);
