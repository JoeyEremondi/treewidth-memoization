#include "VSet.hh"
#include <cstdio>
#include <cstdlib>

//TODO use bitset underneath?

int VSet::maxNumVerts = 64; //TODO sensible default?

VSet::VSet()
	: bitVec(64)
{
	if (maxNumVerts > 64)
	{
		bitVec.resize(maxNumVerts);
	}
	
}


VSet::VSet(const Graph &G)
	: bitVec(64)
{
	auto iterInfo = boost::vertices(G);
	if (maxNumVerts > 64)
	{
		bitVec.resize(maxNumVerts);
	}
	//Just go through and set the bit for each vertex to 1
	for (auto iter = iterInfo.first; iter != iterInfo.second; ++iter)
	{
		bitVec[*iter] = true;
	}

}

VSet::VSet(const VSet &S)
{
	bitVec = S.bitVec;
}

VSet::VSet(const std::vector<Vertex>& vec)
{
	auto loopEnd = vec.end();
	for (auto iter = vec.begin(); iter != loopEnd; ++iter)
	{
		bitVec[*iter] = true;
	}

}



void VSet::members(std::vector<Vertex>& vec) const
{
	auto ourCount = bitVec.count();
	vec.resize(ourCount);

	int vecPos = 0;
	int numAdded = 0;
	for (int i = 0; vecPos < ourCount; ++i)
	{
		//Check if the ith bit is set. If it is, add that to our vector
		if (bitVec[i])
		{
			//std::cout << "Setting membres pos " << vecPos << " to " << i << "\n";
			vec[vecPos] = i;
			vecPos++;
		}

	}
}


Vertex VSet::first() const
{
	return bitVec.find_first();
}



VSet VSet::setUnion(const VSet& that) const
{
	auto newBitVec = this->bitVec | that.bitVec;
	VSet ret;
	ret.bitVec = newBitVec;
	return ret;
}

VSet VSet::setIntersection(const VSet& that) const
{
	auto newBitVec = this->bitVec & that.bitVec;
	VSet ret;
	ret.bitVec = newBitVec;
	return ret;
}

std::string showSet(VSet S) {
	std::ostringstream result;

	result << "{";

	std::vector<Vertex> members;
	S.members(members);

	for (auto iter = members.begin(); iter != members.end(); ++iter)
	{
		result << *iter << " ; ";
	}

	result << "}";

	return result.str();

}
