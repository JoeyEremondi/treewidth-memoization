#include <cstdlib>
#include <iostream>
#include <set>
#include <climits>
#include <algorithm>

#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/breadth_first_search.hpp>
#include <boost/graph/filtered_graph.hpp>

#include <vector>

#include "naive.hh" 

//using namespace std;

//Treewidth is always positive, so we can use this in place of -Inf
const int NO_WIDTH = -1;

int naiveTW(std::set<int> S, Graph G)
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
      int v = *iter;
      S.erase(iter);
      minSoFar = std::min(minSoFar, std::max(naiveTW(S, G), sizeQ(S, v, G) ) );
      S.insert(*iter);
      } 
    return minSoFar;
    
  }
} 


/**
Functor to filter vertices in a graph which are not in the given set
 */
class VertexSetFilter
{
  public:
    std::set<int> S;
    
    VertexSetFilter(std::set<int> inputSet)
    {
	S = inputSet;
    }
    
  bool operator()(const Vertex& v) const
  {
      return S.find(v) != S.end(); // keep all vertx_descriptors greater than 3
  }
};


//Visitor for the BFS we do to find the Q-sets in our naive algorithm
template < typename TVertex, typename TGraph >
class QVisitor : public boost::default_bfs_visitor
{
private:
    int numFound;
    
public:
    
    void discover_vertex(TVertex u, const TGraph & g) const
	{
	    std::cout << u << std::endl;
	}
    int getNumFound()
	{
	    return numFound;
	}
    
	    

  
};

int sizeQ(std::set<int> S, int v, Graph G)
{
    //boost::filtered_graph<Graph> Gfiltered();
    VertexSetFilter myFilter(S);
    typedef boost::filtered_graph<Graph, boost::keep_all, VertexSetFilter > SubGraph;
    SubGraph filteredG(G, boost::keep_all(), myFilter); // (graph, EdgePredicate, VertexPredicate)
    
    QVisitor<Vertex, SubGraph> vis;
    Vertex theV = boost::vertex(v, filteredG);
    //boost::breadth_first_search(filteredG, theV, boost::visitor(vis));
    return -1; //TODO implement  
}


int main()
{
  std::cout << "Hello";
}
