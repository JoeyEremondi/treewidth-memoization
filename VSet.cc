#include "VSet.hh"
#include <cstdio>
#include <cstdlib>

//TODO use bitset underneath?

int VSet::maxNumVerts = 64; //TODO sensible default?

VSet::VSet()
{
	bitVec.resize(maxNumVerts);
}


VSet::VSet(const Graph &G)
{
	auto iterInfo = boost::vertices(G);
	bitVec.resize(boost::num_vertices(G));
	//Just go through and set the bit for each vertex to 1
	for (auto iter = iterInfo.first; iter != iterInfo.second; ++iter)
	{
		bitVec[*iter] = true;

	}

}

VSet::VSet(const VSet &S)
{
	bitVec = S.bitVec;
	//memcpy(bitVec, S.bitVec, MAX_NUM_VERTICES);


}

VSet::VSet(const std::vector<Vertex>& vec)
{
	auto loopEnd = vec.end();
	for (auto iter = vec.begin(); iter != loopEnd; ++iter)
	{
		//bitVec.set(*iter, true);
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
	//std::cout << "MEMBERS size " << vec.size() << "\n";

}

/*
bitSet VSet::getBitVec() const
{
	return bitVec;

}
*/



Vertex VSet::first() const
{
	return bitVec.find_first();
}

Vertex VSet::firstNotContained(int numVerts) const
{
	for (int i = 0; i < numVerts; ++i)
	{
		//Check if the ith bit is set. If it is, add that to our vector
		if (!bitVec[i])
		{
			//std::cout << "Setting membres pos " << vecPos << " to " << i << "\n";

			return i;
		}

	}
}

VSet VSet::setUnion(const VSet& that) const
{
	auto newBitVec = this->bitVec | that.bitVec;
	VSet ret;
	ret.bitVec = newBitVec;
	return ret;
}