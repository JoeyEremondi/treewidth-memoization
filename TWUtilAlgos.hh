#ifndef __TW__UTIL__ALGOS__H
#define __TW__UTIL__ALGOS__H

#include "qset.hh"
#include <vector>

//Find the width of a given permutation
//Useful in finding heuristic upper bounds for treewidth
//TODO check if need copy?
int permutTW(std::vector<Vertex> Svec, Graph G);



#endif
