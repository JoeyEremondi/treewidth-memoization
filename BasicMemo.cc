#include <vector>
#include <set>
#include <cstdlib>

#include "BasicMemo.hh"

BasicMemo::BasicMemo()
{
    storedCalls = new std::map<std::set<Vertex>,int>();
}

BasicMemo::~BasicMemo()
{
    delete storedCalls;
}


    //We just recursively call our TW algorithm in the naive version
int BasicMemo::subTW(std::set<Vertex> S, Graph G) 
{
    auto maybeStored = storedCalls->find(S);
    if ( maybeStored == storedCalls->end())
    {
	auto newEntry = std::pair<std::set<Vertex>, int>(S, memoTW(this, S, G));
	storedCalls->insert(newEntry);
	return newEntry.second;
    } else {
	return maybeStored->second;
    }
    
}


    //Ordered vertices: naive version doesn't order anything
std::vector<Vertex> BasicMemo::orderVertices(std::set<Vertex> S, Graph G)
{
    std::vector<Vertex> v(S.begin(), S.end());
    return v;
}
