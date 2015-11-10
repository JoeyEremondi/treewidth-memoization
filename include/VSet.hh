#ifndef __VSET__HH
#define __VSET__HH


#include <bitset>
#include <vector>
#include <unordered_map>
#include "graphTypes.hh"
#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#include "boost/dynamic_bitset.hpp"


//Memory efficient storage of subsets of vertices in a graph
class VSet
{
public:
	boost::dynamic_bitset<uint64_t> bitVec;

public:
	//Empty set
	VSet();
	//Set of all vertices in a graph
	VSet(const Graph &G);
	//Copy constructor
	VSet(const VSet &S);
	//Convert a vector to a set of vertices
	VSet(const std::vector<Vertex>& vec);

	//Inline operators, shallow wrappers around the bitset operations
	inline void erase(Vertex v) { bitVec[v] = false; }
	inline bool contains(Vertex v) const { return bitVec[v]; };
	inline bool empty() const { return bitVec.none(); }
	inline int size() const { return bitVec.count(); }
	inline void insert(Vertex v) { bitVec[v] = true; }
	inline void addAll(const VSet& that) { bitVec |= that.bitVec; }

	//Return the lowest index of a vertex in this set
	Vertex first() const;

	//Write into the given vector
	void members(std::vector<Vertex>& vec) const;

	//Basic boolean operations
	VSet setUnion(const VSet& that) const;
	VSet setIntersection(const VSet& that) const;

	//We improve performance by setting this globally
	//And always initializing new VSets to this value
	static int maxNumVerts;
	



};

//Allow our VSets to be stored in trees or hashtables
namespace std {
	template <> struct hash<VSet>
	{
		size_t operator()(const VSet & S) const
		{
			return boost::hash_value(S.bitVec.m_bits);
		}
	};
}

inline bool operator< (const VSet& lhs, const VSet& rhs) {
	return lhs.bitVec < rhs.bitVec;
}

inline bool operator> (const VSet& lhs, const VSet& rhs) { return rhs < lhs; }
inline bool operator<=(const VSet& lhs, const VSet& rhs) { return !(lhs > rhs); }
inline bool operator>=(const VSet& lhs, const VSet& rhs) { return !(lhs < rhs); }
inline bool operator==(const VSet& lhs, const VSet& rhs) { return lhs.bitVec == rhs.bitVec; }

//Useful for pretty-printing
std::string showSet(VSet S);

#endif
