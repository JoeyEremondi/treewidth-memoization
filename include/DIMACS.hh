#ifndef __DIMACS__H
#define __DIMACS__H

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_match.hpp>

#include "qset.hh"

//Given a string stream, and a reference to a graph
//Read the string as a DIMACS graph file, and edit the given
//graph to store the one described in the file
bool read_coloring_problem(std::istream& dimacs, Graph& g);

#endif
