#ifndef __VSET__HH
#define __VSET__HH

//We gain speed by using fixed-size bitsets to store sets of vertices
//This can be changed using compile-time parameters
//#ifndef MAX_NUM_VERTICES
//#define MAX_NUM_VERTICES 64
//#endif


#include <bitset>
#include <vector>
#include <unordered_map>
#include "graphTypes.hh"
#define BOOST_DYNAMIC_BITSET_DONT_USE_FRIENDS
#include "boost/dynamic_bitset.hpp"

typedef boost::dynamic_bitset<unsigned long> bitSet;

//Memory efficient storage of subsets of vertices in a graph
class VSet
{
public:
	//bitSet bitVec = 0;
	boost::dynamic_bitset<unsigned long long> bitVec;
	//int currentSize = 0;

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
	//unsigned long long asInt() const;

	inline void erase(Vertex v) { bitVec[v] = false; }
	inline bool contains(Vertex v) const { return bitVec[v]; };
	inline bool empty() const { return bitVec.none(); }
	inline int size() const { return bitVec.count(); }
	inline void insert(Vertex v) { bitVec[v] = true; }
	inline void addAll(const VSet& that) { bitVec |= that.bitVec; }


	Vertex first() const;
	Vertex firstNotContained(int numVerts) const;

	//Write into the given vector
	void members(std::vector<Vertex>& vec) const;
	//bitSet getBitVec() const;

	std::size_t operator()() const;

	VSet setUnion(const VSet& that) const;

	static int maxNumVerts;
	



};

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

#endif
