#ifndef __DAWG__HH__
#define __DAWG__HH__

#include "VSet.hh"

typedef uint32_t State;

struct StackElem
{
	State state;
	int depth;
	VSet set;
	std::vector<State> statePath;
};


struct StateSignature
{
	State s1;
	State s2;
};
//Hash for pairs of states
namespace std {
	template <> struct hash<StateSignature>
	{
		size_t operator()(const StateSignature & ss) const
		{
			std::size_t seed = 0;
			boost::hash_combine(seed, ss.s1);
			boost::hash_combine(seed, ss.s2);
			return seed;
		}
	};
}
inline bool operator==(const StateSignature& lhs, const StateSignature& rhs)
{
	return (lhs.s1 == rhs.s1) && (lhs.s2 == rhs.s2);
}
inline bool operator<(const StateSignature& lhs, const StateSignature& rhs)
{
	return (lhs.s1 < rhs.s1) || (lhs.s1 == rhs.s1 && lhs.s2 == rhs.s2);
} 
//typedef std::pair<State, State> StateSignature;

class DAWG
{
private:
	uint64_t nextState = 2;
	State initial = 1;

	std::unordered_map<State, State>* delta0; 
	std::unordered_map<State, State>* delta1;

	const State SINK = 0;
	std::vector<StackElem> iterStack;
	StackElem currentStack;
	int length;

	//Based off of depth-first minimization, stringology paper //TODO cite
	std::unordered_map<StateSignature, State> Register;
	std::unordered_map<State, State> StateMap;

protected:
	int pathsToEndFrom(int depth, State q);
	State newState();
	void addTransition(int depth, State from, State to, bool readLetter);
	void minimize();
	void minimizeHelper(int layer, State q);
	void deleteState(int layer, State q);
	void insert(VSet word);

	std::vector<std::string> DAWG::wordSetHelper(int depth, State q);

	inline void setTransition(int layer, State q, bool bit, State tnext)
	{
		if (bit)
		{
			delta1[layer][q] = tnext;
		}
		else
		{
			delta0[layer][q] = tnext;
		}
	}
	

public:
	int numTransitions();
	
	std::string asDot();
	DAWG();
	~DAWG();
	int size();
	std::vector<std::string> DAWG::wordSet();
	
	void insertSafe(VSet word);
	bool contains(VSet word);
	inline State delta(int depth, State q, bool bitRead) {
		if (depth >= length)
		{
			return SINK;
		}
		if (bitRead)
		{
			auto searchInfo = delta1[depth].find(q);
			if (searchInfo == delta1[depth].end())
			{
				return SINK;
			}
			return searchInfo->second;
		}
		else
		{
			auto searchInfo = delta0[depth].find(q);
			if (searchInfo == delta0[depth].end())
			{
				return SINK;
			}
			return searchInfo->second;
		}
	}

	inline StateSignature sig(int layer, State q)
	{
		return{delta(layer, q, false), delta(layer, q, true)};
	}

	void initIter();
	VSet nextIter();
	bool iterDone();
	bool empty();
};

#endif