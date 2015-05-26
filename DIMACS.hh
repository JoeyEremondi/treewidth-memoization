#ifndef __DIMACS__H
#define __DIMACS__H

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_match.hpp>

#include "qset.hh"

//std::pair<int, int> coloring_problem_size(std::istream& dimacs);

bool read_coloring_problem(std::istream& dimacs, Graph& g);

#endif
