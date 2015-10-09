#ifndef __DAWG__HH__
#define __DAWG__HH__

#include "VSet.hh"
#include <unordered_set>
#include <unordered_map>

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
	bool isTrie = true;
	State nextState = 3;
	State initial = 2;

	std::vector<std::unordered_map<State, State>> delta0;
	std::vector<std::unordered_map<State, State>> delta1;

	std::unordered_map<State, int> valueDelta;

	//std::unordered_map<State, State>::iterator* d0end;
	//std::unordered_map<State, State>::iterator* d1end;
	//std::unordered_map<State, int>::iterator valueEnd;

	const State SINK = 0;
	const State FINAL = 1;



	int length;

	//Based off of depth-first minimization, stringology paper //TODO cite
	std::unordered_map<StateSignature, State>* Register;
	//std::unordered_map<StateSignature, State>::iterator* RegisterEnd;

	std::unordered_map<int, State> EndRegister;
	std::unordered_map<int, State>::iterator EndRegisterEnd;

	std::unordered_map<State, State>* StateMap;
	//std::unordered_map<State, State>::iterator* StateMapEnd;

protected:
	int pathsToEndFrom(int depth, State q);
	State newState();
	void addTransition(int depth, State from, State to, bool readLetter);
	void minimize();
	State minimizeHelper(int layer, State q);
	void deleteState(int layer, State q);
	void insertIntoEmpty(VSet word, int tw);

	std::vector<std::string> wordSetHelper(int depth, State q);



	/*
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
	} */


public:
	const int NOT_CONTAINED = -1;
	void clear();
	int numTransitions();

	void trim();

	std::string asDot();
	DAWG();
	void copyFrom(const DAWG& oldDawg);
	~DAWG();
	int size();
	std::vector<std::string> wordSet();

	void insert(VSet word, int tw);
	int contains(VSet word);
	State delta(int depth, State q, bool bitRead) {
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



	//Adapted from https://gist.github.com/jeetsukumaran/307264
	class iterator
	{
	public:
		typedef iterator self_type;
		typedef std::pair<VSet, int> value_type;
		typedef std::pair<VSet, int>& reference;
		typedef std::pair<VSet, int>* pointer;
		typedef std::forward_iterator_tag iterator_category;
		typedef int difference_type;

		self_type operator++() { return this->nextIter(); };
		reference operator*() { return currentPair; }
		pointer operator->() { return &currentPair; }
		//We cheat: only empty iterators are equal
		bool operator==(const self_type& rhs) { return iterStack.empty() && rhs.iterStack.empty(); }
		bool operator!=(const self_type& rhs) { return !(iterStack.empty() && rhs.iterStack.empty()); }
		iterator(StackElem elem, DAWG* super);
		iterator() : iterStack() { super = NULL; };
	protected:
		DAWG* super;
		self_type nextIter();
		std::vector<StackElem> iterStack;
		StackElem currentStack;
		std::pair<VSet, int> currentPair;
	};

	iterator begin();
	iterator end();
	bool empty();
};

#endif
