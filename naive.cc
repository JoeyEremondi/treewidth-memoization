#include <cstdlib>
#include <iostream>
#include <set>
#include <climits>
#include <algorithm>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/filtered_graph.hpp>
#include <boost/graph/random.hpp>
#include <boost/random/mersenne_twister.hpp> 
#include <boost/graph/graphviz.hpp>

#include <vector>
#include <utility>

#include "naive.hh" 

//using namespace std;

//Treewidth is always positive, so we can use this in place of -Inf
const int NO_WIDTH = -1;


std::string showSet(std::set<Vertex> S) {
std::ostringstream result;  

result << "{";


for (auto iter = S.begin(); iter != S.end(); iter++)
{
result << *iter << " ; ";
}

result << "}";

return result.str();

}



int naiveTW(std::set<Vertex> S, Graph G)
{
  if (S.empty())  
  {
    return NO_WIDTH;
  }
  else
  {
    int minSoFar = INT_MAX;

    for (auto iter = S.begin(); iter != S.end(); iter++)
    {
      Vertex v = *iter;
      std::set<Vertex> S2(S);

      S2.erase(v);

      int subTW = naiveTW(S2, G);
int qVal = sizeQ(S2, v, G);



std::cout << "S2 is " << showSet(S2) << std::endl;
std::cout << "subTW " << subTW << std::endl;
std::cout << "qVal " << qVal << std::endl;

      minSoFar = std::min(minSoFar, std::max(subTW, qVal ) );
      } 
    return minSoFar;
    
  } 
} 


/**
Functor to remove vertices in a graph which are in the given set
 */
class QFilter
{
  public:
    std::set<Vertex> S;
    QFilter() = default;
    
    QFilter(std::set<Vertex> inputSet)
    {
	S = inputSet;
    }
    
  bool operator()(const Vertex& v) const
  {
      return S.find(v) == S.end(); // keep all vertx_descriptors greater than 3
  }
};


//Visitor for the BFS we do to find the Q-sets in our naive algorithm
template < typename TVertex, typename TGraph >
class QVisitor : public boost::default_bfs_visitor
{
    int numFound = 0;
    Vertex startVertex;



    
public:

    QVisitor() = default;
    
    QVisitor(Vertex v)
    {
	startVertex = v;
    }
    
    void discover_vertex(TVertex u, const TGraph & g) 
	{
            if (u != startVertex){
std::cout << "Found vertex Q" << std::endl;
                numFound++;
            }

	}
    int getNumFound()
	{
	    return numFound;
	}

};

int sizeQ(std::set<Vertex> S, Vertex v, Graph G)
{
    //boost::filtered_graph<Graph> Gfiltered();
    QFilter myFilter(S);
    typedef boost::filtered_graph<Graph, boost::keep_all, QFilter > SubGraph;
    SubGraph filteredG(G, boost::keep_all(), myFilter); // (graph, EdgePredicate, VertexPredicate)
    
QVisitor<Vertex, SubGraph> vis(v);
    //Vertex theV = boost::vertex(v, filteredG);
    boost::breadth_first_search(filteredG, v, boost::visitor(vis));
std::cout << "Q size: " << vis.getNumFound() << std::endl;
    return vis.getNumFound();

}


int main()
{
  
    Graph g;
    boost::mt19937 rng;
    boost::generate_random_graph(g, 2, 1, rng, true, true);

boost::write_graphviz(std::cout, g);


    auto iterInfo = boost::vertices(g);
std::set<Vertex> S;
for (auto iter = iterInfo.first; iter != iterInfo.second; iter++ )
    {
S.insert(*iter);
    }


std::cout << "Treewidth: " << naiveTW(S, g) << std::endl;
    return EXIT_SUCCESS;

}
