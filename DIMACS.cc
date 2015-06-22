#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_match.hpp>


#include "qset.hh"
#include "DIMACS.hh"

/*
std::pair<int, int> coloring_problem_size(std::istream& dimacs) {
    size_t vertices = 0, edges = 0;

    std::string line;
    while (getline(dimacs, line))
    {	
        std::istringstream iss(line);
        char ch;
        if (iss >> ch)
        {
            size_t from, to;
            std::string format;

	    
	    

            switch(ch) {
                case 'p': 
                    if (iss >> format >> vertices >> edges) {
                        std::pair<int, int> p(vertices, edges);
			return p;
			
                    }
                    break;
                default: break;
            }
        }
    }

    std::pair<int, int> p(-1, -1);
    
    return p;
    } */


//Taken from http://stackoverflow.com/questions/30415388/how-to-read-dimacs-vertex-coloring-graphs-in-c/30446685#30446685
//Special thanks to user sehe
bool read_coloring_problem(std::istream& dimacs, Graph& g) {
    size_t vertices = 0, edges = 0;

    
    std::map<Vertex, Vertex> newNums;
    Vertex nextVertex = 0;
    
    std::string line;
    while (getline(dimacs, line))
    {
        std::istringstream iss(line);
        char ch;
        if (iss >> ch)
        {
            size_t from, to;
            std::string format;

            switch(ch) {
                case 'c': break;
                case 'p': 
                    if (vertices||edges) return false;
                    if (iss >> format >> vertices >> edges) {
                        if ("edge" != format) return false;
                    }
                    break;
                case 'e': 
                    if (edges-- && (iss >> from >> to))
		    { 
			Vertex newTo, newFrom;
			auto findFrom = newNums.find(from);
			
			//Renumber our vertices if needed

			if (findFrom != newNums.end() )
			{
			    newFrom = (*findFrom).second;
			} else {
			    newFrom = nextVertex;
			    nextVertex++;
			    newNums.insert(std::pair<Vertex, Vertex>(from, newFrom));
			}

			auto findTo = newNums.find(to);
			if (findTo != newNums.end() )
			{
			    newTo = (*findTo).second;
			} else {
			    newTo = nextVertex;
			    nextVertex++;
			    newNums.insert(std::pair<Vertex, Vertex>(to, newTo));
			    
			}
			
			
			if (add_edge(newFrom, newTo, g).second)
			{
			    //std::cout << "Adding edge " << newFrom << " " << newTo << "\n";
			    break;
			}
			
		    }
		    return false;
		    
	        case 'n':
	        case 'x':
		    //Skip node info
		    break;
		    
                default: 
                    return false;
            }
        }
    }

    return !(edges || !dimacs.eof());
}
