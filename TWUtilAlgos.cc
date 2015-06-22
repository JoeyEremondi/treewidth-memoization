#include "qset.hh"
#include <vector>
#include <map>

#include "TWUtilAlgos.hh"
#include "AbstractMemo.hh"

#include <boost/graph/subgraph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

int permutTW(std::vector<Vertex> Svec, Graph G) {
    int tw = NO_WIDTH;
    
    while (!Svec.empty())
    {
	auto current = Svec.back();
	Svec.pop_back();
	VSet S(Svec);
	
	int qval = sizeQ(S, current, G);
	

        tw = std::max(tw, qval);
	
    }
    return tw;
}

std::pair<Graph, std::map<Vertex, Vertex>> complement_graph(const Graph& G)
{
    auto vertexInfo = boost::vertices(G);
    //Start GC as identical to G, gives us all the same vertices
    Graph GC;

    std::map<Vertex, Vertex> vmap; 

    //Add all vertices from G to GC, and store them in a vertex map
    for (auto u = vertexInfo.first; u != vertexInfo.second; u++)
    {
	Vertex newVert = boost::add_vertex(GC);
	//std::cout << "Making graph pair " << *u << " and " << newVert << "\n";
	vmap[*u] = newVert ;
    }
    

    //Add all the remaining edges to our compliment
    for (auto u = vertexInfo.first; u != vertexInfo.second; u++)
    {
	for (auto v = vertexInfo.first; v != vertexInfo.second; v++)
	{
	    if (*u < *v && ( edge(*u, *v, G).second == false))
	    {
		//std::cout << "Adding comp edge " << *u << " and " << *v << "\n";
		//std::cout << "Comp numbers: " << vmap[*u] << " and " << vmap[*v] << "\n";
		boost::add_edge(vmap[*u], vmap[*v], GC);
	    }
	    
	}
	
    }
    
    //Return our key-value set for the map
    //http://stackoverflow.com/questions/8321316/flip-map-key-value-pair
    std::map<Vertex, Vertex> reversed;

    for (auto i = vmap.begin(); i != vmap.end(); ++i)
	reversed[i->second] = i->first;

    std::pair<Graph, std::map<Vertex, Vertex>> ret(GC, reversed);
    return ret;
    
}


int subgraphDegree(Vertex v, VSet S, Graph G)
{
    int ret = 0;
    
    //std::cout << "Getting adj vertices for " << v << "\n";
    
    auto neighbourSet = boost::adjacent_vertices(v, G);
    
    for (auto iter = neighbourSet.first; iter != neighbourSet.second; iter++)
    {
	//std::cout << "Looking at neighbour " << *iter << "\n";
	
	if (S.contains(*iter))
	{
	    ret++;
	}
	
    }
    
    return ret;
    
}


VSet maxIndSetHelper(VSet S, Graph G)
{
    //std::cout << "Max IS: size " << S.size() << "\n";
    //Base case
   if (S.size() <= 1)
    {
	//std::cout << "Base case\n";
	return S;
    }
    
    
    auto members = S.members();
    Vertex minDegreeVert = members[0];
    int minDeg = subgraphDegree(members[0], S, G);

    //std::cout << "Set members: " << members;

    for (auto iter = members.begin(); iter != members.end(); iter++ )
    {
	//std::cout << "Checking degree for " << *iter << "\n";
	int currentDeg = subgraphDegree(*iter, S, G);
	if (minDeg > currentDeg )
	{
	    minDegreeVert = *iter;
	    minDeg = currentDeg;
	}
    }
    

    //Try finding max IS with the min-degree vertex removed
    auto SV = S;
    //Erase each neighbour of the vertex, and the vertex itself
    auto svNeighbs = boost::adjacent_vertices(minDegreeVert, G );
    for (auto neighb = svNeighbs.first; neighb != svNeighbs.second; neighb++)
    {
	SV.erase(*neighb);
    }
    SV.erase(minDegreeVert);
    VSet bestFound = maxIndSetHelper(SV, G);
    bestFound.insert(minDegreeVert);
    
    //Then, iterate through each neighbour of V, and try having it in the IS
    auto neighbourSet = boost::adjacent_vertices(minDegreeVert, G );
    for (auto iter = neighbourSet.first; iter != neighbourSet.second; iter++)
    {
	//Ignore vertices not in the current subgraph
	if (S.contains(*iter))
	{
	    auto SNew = S;
	    //Erase each neighbour of this neighbour
	    auto nnSet = boost::adjacent_vertices(minDegreeVert, G );
	    for (auto neighb = nnSet.first; neighb != nnSet.second; neighb++)
	    {
		SNew.erase(*neighb);
	    }
	    //And erase the neighbour itself
	    SNew.erase(*iter);
	    VSet subFound = maxIndSetHelper(SNew, G);
	    subFound.insert(*iter);
	    //std::cout << "Returning to " << S.size() << "\n";
	    if (bestFound.size() < subFound.size() )
	    {
		bestFound = subFound;
	    }
	}
	
	
    }
    //std::cout << "Max IS: returning " << S.size() << "\n";
    return bestFound;
    
}

VSet exactMaxIndSet(Graph G)
{
    VSet S(G);
    return maxIndSetHelper(S, G);
    
}


VSet exactMaxClique(Graph G)
{
    auto compInfo = complement_graph(G);
    VSet iset =  exactMaxIndSet(compInfo.first);
    auto iVec = iset.members();

    //Translate the compliment vertices into our vertices
    VSet ret;
    
    for (auto iter = iVec.begin(); iter != iVec.end(); iter++)
    {
	ret.insert(compInfo.second[*iter]);
    }

    return ret;
    
}


//Approximate algorithms for clique and IS
//using the min-degree heuristic
//We can do this quickly using iteration
VSet approxIndSetHelper(VSet Sarg, Graph G)
{
    VSet S = Sarg;
    VSet returnSet;
    

    //Base case
   while (S.size() > 0)
    {    
	auto members = S.members();
	Vertex minDegreeVert = members[0];
	int minDeg = subgraphDegree(members[0], S, G);

	for (auto iter = members.begin(); iter != members.end(); iter++ )
	{
	    int currentDeg = subgraphDegree(*iter, S, G);
	    if (minDeg > currentDeg )
	    {
		minDegreeVert = *iter;
		minDeg = currentDeg;
	    }
	}

	//Assume the Max-IS contains the min-degree vertex
	//Erase each neighbour of the vertex, and the vertex itself
	auto svNeighbs = boost::adjacent_vertices(minDegreeVert, G );
	for (auto neighb = svNeighbs.first; neighb != svNeighbs.second; neighb++)
	{
	    S.erase(*neighb);
	}
	S.erase(minDegreeVert);
	//The min-degree vertex is in our independent set, the neighbours are not
	returnSet.insert(minDegreeVert);
    }
   
   
    return returnSet;
    
}

VSet approxMaxIndSet(Graph G)
{
    VSet S(G);
    return approxIndSetHelper(S, G);
    
}

VSet approxMaxClique(Graph G)
{
    auto compInfo = complement_graph(G);
    VSet iset =  approxMaxIndSet(compInfo.first);
    auto iVec = iset.members();

    //Translate the compliment vertices into our vertices
    VSet ret;
    
    for (auto iter = iVec.begin(); iter != iVec.end(); iter++)
    {
	ret.insert(compInfo.second[*iter]);
    }

    return ret;
    
}

//TODO is there a smarter way to do this with boost subgraphs?
int subgraphTWLowerBound(VSet S, Graph G)
{    
    auto memb = S.members();
    
    Graph Gs = G;

    auto vinfo = boost::vertices(G);
    for (auto iter = vinfo.first; iter != vinfo.second; iter++)
    {
	if (!S.contains(*iter))
	{
	    boost::remove_vertex(*iter, Gs);
	}
	
    }    
    VSet aClique = approxMaxClique(Gs);
    
    //TODO more advanced techniques?
    return aClique.size();
}


