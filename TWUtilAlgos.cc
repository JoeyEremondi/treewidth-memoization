#include "qset.hh"
#include <vector>
#include <map>

#include "TWUtilAlgos.hh"
#include "AbstractMemo.hh"

#include <boost/graph/subgraph.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_utility.hpp>

#include <boost/graph/graphviz.hpp>

int permutTW(VSet vsetArg, const std::vector<Vertex>& Svec, const Graph& G) {
    int tw = NO_WIDTH;
    VSet vs = vsetArg;
    
    for(auto current = Svec.begin(); current != Svec.end(); current++)
    {
	vs.erase(*current);
	int qval = sizeQ(vs, *current, G);	
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
	    //std::cout << "Edge Info: " << *u << " " << *v << " " << edge(*u, *v, G).second << "\n";
	    
	    if (*u < *v && ! ( edge(*u, *v, G).second))
	    {
		//std::cout << "Adding comp edge " << vmap[*u] << " and " << vmap[*v] << "\n";
		//std::cout << "Comp numbers: " << vmap[*u] << " and " << vmap[*v] << "\n";
		boost::add_edge(vmap[*u], vmap[*v], GC);
	    }
	    
	}
	
    }

    //std::cout << "Done comp\n";
    
    //Return our key-value set for the map
    //http://stackoverflow.com/questions/8321316/flip-map-key-value-pair
    std::map<Vertex, Vertex> reversed;

    for (auto i = vmap.begin(); i != vmap.end(); ++i)
	reversed[i->second] = i->first;

    std::pair<Graph, std::map<Vertex, Vertex>> ret(GC, reversed);
    return ret;
    
}


int subgraphDegree(Vertex v, VSet S, const Graph& G)
{
    int ret = 0;
    
    VSet seenNeighbours;
    
    //std::cout << "Getting adj vertices for " << v << "\n";
    
    auto neighbourSet = boost::adjacent_vertices(v, G);
    
    for (auto iter = neighbourSet.first; iter != neighbourSet.second; iter++)
    {
	//std::cout << "Looking at neighbour: " << "\n";
	//std::cout<< *iter << "\n";
	
	if (S.contains(*iter) && !seenNeighbours.contains(*iter))
	{
	    ret++;
	    seenNeighbours.insert(*iter);
	}
	
    }
    
    return ret;
    
}

//TODO this is not returning cliques
VSet maxIndSetHelper(VSet S, const Graph& G)
{
    //std::cout << "Max IS: size " << S.size() << "\n";
    //Base case
   if (S.size() <= 1)
    {
	//std::cout << "Base case\n";
	return S;
    }
    
   //std::cout << "Past base case: size " << S.size() << "\n";

   //auto members = S.members();

    std::pair<Vertex, int> minDegInfo = minDegreeSubgraphVert(S, G);
    
    Vertex minDegreeVert = minDegInfo.first;
    int minDeg = minDegInfo.second;


    //Try finding max IS with the min-degree vertex removed
    auto SV = S;
    //Erase each neighbour of the vertex, and the vertex itself
    //std::cout << "Size " << S.size() << " choosing " << minDegreeVert << "\n";
    SV.erase(minDegreeVert);
    auto svNeighbs = boost::adjacent_vertices(minDegreeVert, G );
    for (auto minNeighb = svNeighbs.first; minNeighb != svNeighbs.second; minNeighb++)
    {
	//std::cout << "Size " << S.size() << " erasing " << *minNeighb << "\n";
	
	SV.erase(*minNeighb);
    }
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
	    //std::cout << "Size " << S.size() << " choosing " << *iter << "\n";
	    //And erase the neighbour itself
	    SNew.erase(*iter);

	    //Erase the min-degree vertex
	    //TODO why do we need this?
	    //std::cout << "Size " << S.size() << " erasing " << minDegreeVert << "\n";
	    SNew.erase(minDegreeVert);

	    auto nnSet = boost::adjacent_vertices(minDegreeVert, G );
	    for (auto nneighb = nnSet.first; nneighb != nnSet.second; nneighb++)
	    {
		//std::cout << "Size " << S.size() << " erasing " << *nneighb << "\n";
		SNew.erase(*nneighb);
	    }
	    
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

VSet exactMaxIndSet(const Graph& G)
{
    VSet S(G);
    return maxIndSetHelper(S, G);
    
}


VSet exactMaxClique(const Graph& G)
{
    auto compInfo = complement_graph(G);
    //std::cout << "Graph complement:\n";
    //boost::write_graphviz(std::cout, compInfo.first);

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
VSet approxIndSetHelper(VSet Sarg, const Graph& G)
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

VSet approxMaxIndSet(const Graph& G)
{
    VSet S(G);
    return approxIndSetHelper(S, G);
    
}

VSet approxMaxClique(const Graph& G)
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
int subgraphTWLowerBound(VSet S, const Graph& G, const Graph& GC)
{    
    VSet aClique = approxIndSetHelper(S, GC);
    
    //TODO more advanced techniques?
    return aClique.size();
}

std::pair<Vertex, int> minDegreeSubgraphVert(VSet S, const Graph& G)
{
    auto members = S.members();
    
    //std::cout << "Members size " << members.size() << "\n";

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

    return std::pair<Vertex, int>(minDegreeVert, minDeg );
}

//Fails if S doesn't have at least 2 vertices
std::pair<Vertex, int> minDegreeVertExcept(Vertex v, VSet S, const Graph& G)
{
    auto members = S.members();
    
    Vertex minDegreeVert;
    int minDeg;

    if (v == members[0])
    {
	minDegreeVert = members[1];
	minDeg = subgraphDegree(members[1], S, G);
    } else {
	minDegreeVert = members[0];
	minDeg = subgraphDegree(members[0], S, G);
    }

    for (auto iter = members.begin(); iter != members.end(); iter++ )
    {
	//std::cout << "Checking degree for " << *iter << "\n";
	int currentDeg = subgraphDegree(*iter, S, G);
	if (*iter != v && minDeg > currentDeg )
	{
	    minDegreeVert = *iter;
	    minDeg = currentDeg;
	}
    }

    return std::pair<Vertex, int>(minDegreeVert, minDeg );
}

int MMD(VSet S, const Graph& G)
{
    //std::cout << "Min Degree vertex:" << minDeg(S, G) << "\n";
    
    VSet H = S;
    int maxmin = 0;
    while (H.size() >= 2)
    {
	Vertex v = minDegreeSubgraphVert(H, G).first;
	//std::cout << "maxmin v degree " << subgraphDegree(v, H, G) << "\n";
	maxmin = std::max(maxmin, subgraphDegree(v, H, G));
	H.erase(v);
    }

    return maxmin;
    
}

int minDeg(VSet S, const Graph& G)
{
    auto memb = S.members();
    int minDeg = S.size();
    for (auto iter = memb.begin(); iter != memb.end(); iter++ )
    {
	minDeg = std::min(minDeg, subgraphDegree(*iter, S, G));
    }
    

    return minDeg;
    
}



//TODO fix this
//Based on Algorithm 2 from the lower bounds paper
int d2degen(VSet Sarg, const Graph& G)
{
    VSet S = Sarg;
    
    int d2D = 0;
    auto vertInfo = boost::vertices(G);
    for (auto iter = vertInfo.first; iter != vertInfo.second; iter++)
    {
	VSet H = G;
	while (H.size() >= 2)
	{
	    Vertex w = minDegreeVertExcept(*iter, H, G).first;
	    d2D = std::max(d2D, subgraphDegree(*iter, H, G)); //TODO *iter or w?
	    H.erase(w);
	}
	
    }
    return d2D;
    
}
