#ifndef __DAWG__HH__
#define __DAWG__HH__

#include "VSet.hh"
#include <unordered_set>
#include <unordered_map>

typedef uint32_t State;

/////////////////////////////////
//Struct, Hash and comparison for pairs of states
//We use these in our minimization algorithm
struct StateSignature
{
	State s1;
	State s2;
};

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


//Miniature class to wrap around a counter, getting unused state numbers
class StatePool
{
private:
	int counter = 2;
public:
	inline int newState() { int ret = counter; counter++; return ret; }
	inline void undo() { counter--; };
};

//A class for storing a sets of vertices, along with their TW value
// in a minimal finite deterministic automaton.
//We store "words" which contain boolean letters, and a final "letter"
//Which is the TW value for the set
class DAWG
{
public:
	//Construct and empty DAWG, containing no words
	DAWG();
	~DAWG();


	//How large do we allow our staging area to get before minimizing?
	static const int ABS_MAX_TRANSITIONS = 50000000;
	int maxTransitions = ABS_MAX_TRANSITIONS;
	//Special TW value for sets not in the DAWG
	const int NOT_CONTAINED = -1;

	//Remove all words from this DAWG
	void clear();

	//Return the number of edges (transitions) in this DAWG (when viewed as an automaton)
	//Does not include the staging area
	int numTransitions();

	//Remove unreachable states and states which cannot reach the final state
	void trim();

	//Make a graphviz string for this DAWG
	std::string asDot();

	//Copy an old DAWG into this DAWG
	void copyFrom(const DAWG& oldDawg);

	//Return the number of words stored in the DAWG
	int size();

	//Iterate through the entire word set of the DAWG and return them as a vector
	//Is very slow, should only be used for debugging
	std::vector<std::string> wordSet();

	//Insert a word into this DAWG. Likely fast, but can trigger union with the staging area
	void insert(VSet word, int tw);

	//Return the TW value if this DAWG contains a given set
	//Or return NOT_CONTAINED if it does not contain it
	int contains(VSet word);


private:
	//Pool of states used internally
	StatePool statePool;

	//Internal constructor: we give NULL to create a staging area
	//Otherwise, we give the address of the staging area for this DAWG
	DAWG(DAWG* staging);

	//Is this a true DAWG, or just a staging area?
	bool isTrie = false;

	//The first value we give to our initial state
	const int startInitial = 2;
	//The current value of our initial state
	State initial = startInitial;

	//Vector of hashtables storing outgoing transitions for states
	//At distance i from the initial state
	std::vector<std::unordered_map<State, State>> delta0;
	std::vector<std::unordered_map<State, State>> delta1;

	//Hashtable storing which TW values are reachable
	//From states in our final layer
	std::unordered_map<State, int> valueDelta;


	const State SINK = 0;
	const State FINAL = 1;


	//How long are the "words" we store?
	//Is equal to the number of vertices in our graph
	int length;

	//Internal variables for minimization
	//Based off of depth-first minimization, stringology paper
	std::unordered_map<StateSignature, State>* Register;
	std::unordered_map<int, State> EndRegister;
	std::unordered_map<int, State>::iterator EndRegisterEnd;
	std::unordered_map<State, State>* StateMap;

protected:
	//Used for counting the number of words in a set
	int pathsToEndFrom(int depth, State q);

	//Minimize this DAWG (as a DFA)
	void minimize();
	//Recursive function called by minimize
	State minimizeHelper(int layer, State q);
	//Remove a state and clean up its transitions
	void deleteState(int layer, State q);
	//Insert a word into this DAWG, assuming the DAWG is a trie
	void insertIntoEmpty(VSet word, int tw);

	//Helper for iterating through all words in the DAWG
	std::vector<std::string> wordSetHelper(int depth, State q);

	//Take all words sotred in the "staging area" of this DAWG
	//And add them into our transition set using a product construction
	void unionWithStaging();
	//The trie which stores words to be added
	DAWG* stagingArea;


	//Utility function for looking up a transition for a state and letter
	//And returning SINK if no such transition exists
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

	//Return the "signature" of a state, i.e. the two (possibly SINK) states
	//It has transitions to
	inline StateSignature sig(int layer, State q)
	{
		return{ delta(layer, q, false), delta(layer, q, true) };
	}

public:
	////////////////////
	//Iterator to go through all words stored in a DAWG
	//Adapted from https://gist.github.com/jeetsukumaran/307264
	class iterator
	{
		//What we store on our stack while doing our DFS
		struct StackElem
		{
			State state;
			int depth;
			VSet set;
			std::vector<State> statePath;
		};



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
