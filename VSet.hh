#ifndef __VSET__HH
#define __VSET__HH

//We gain speed by using fixed-size bitsets to store sets of vertices
//This can be changed using compile-time parameters
#ifndef MAX_NUM_VERTICES
#define MAX_NUM_VERTICES 64
#endif


#include <bitset>
#include <vector>
#include <unordered_map>
#include "graphTypes.hh"

typedef std::bitset<MAX_NUM_VERTICES> bitSet;

//Memory efficient storage of subsets of vertices in a graph
class VSet
{
protected:
//bitSet bitVec = 0;
bool bitVec[MAX_NUM_VERTICES] = { false };
int currentSize = 0;

public:
    //Empty set
    VSet();
    //Set of all vertices in a graph
    VSet(const Graph &G);
    //Copy constructor
    VSet(const VSet &S);
    //Convert a vector to a set of vertices
    VSet(const std::vector<Vertex>& vec);
    
    //Convert a vector to its integer representation
    unsigned long asInt() const;

    void erase(Vertex v);
    bool contains(Vertex v ) const;
    bool empty() const ;
    int size() const;
    void insert(Vertex v);
    
//Write into the given vector
    void members(std::vector<Vertex>& vec) const;
//bitSet getBitVec() const;
    
    std::size_t operator()() const;
    

    
};

namespace std {
  template <> struct hash<VSet>
  {
    size_t operator()(const VSet & S) const
    {
return S.asInt();
    }
  };
}

inline bool operator< (const VSet& lhs, const VSet& rhs){
return lhs.asInt() < rhs.asInt();

    
}
inline bool operator> (const VSet& lhs, const VSet& rhs){return rhs < lhs;}
inline bool operator<=(const VSet& lhs, const VSet& rhs){return !(lhs > rhs);}
inline bool operator>=(const VSet& lhs, const VSet& rhs){return !(lhs < rhs);}
inline bool operator==(const VSet& lhs, const VSet& rhs){return lhs.asInt() == rhs.asInt();}
    

    
#endif
