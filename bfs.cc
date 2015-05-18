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

//Adapted from http://stackoverflow.com/questions/14470566/how-to-traverse-graph-in-boost-use-bfs
typedef boost::adjacency_list < boost::vecS, boost::vecS, boost::undirectedS > Graph;
typedef typename boost::graph_traits<Graph>::edge_descriptor Edge;
typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex; 

/**
Functor to filter vertices in a graph which are not in the given set
 */
class VertexSetFilter
{
  public:
    std::set<Vertex> S;
    
    VertexSetFilter(std::set<Vertex> inputSet)
    {
	S = inputSet;
    }
    
  bool operator()(const Vertex& v) const
  {
      return S.find(v) != S.end(); // keep all vertx_descriptors greater than 3
  }
};


template < typename TVertex, typename TGraph >
class MyVisitor : public boost::default_bfs_visitor
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

int restrictedBFS(std::set<Vertex> S, Vertex v, Graph G)
{
    VertexSetFilter myFilter(S);
    typedef boost::filtered_graph<Graph, boost::keep_all, VertexSetFilter > SubGraph;
    SubGraph filteredG(G, boost::keep_all(), myFilter);
    
    MyVisitor<Vertex, SubGraph> vis;
    //won't compile when this line is uncommented
    boost::breadth_first_search(filteredG, v, boost::visitor(vis));
    return -1; 
}


int main()
{
  std::cout << "Hello";
}
