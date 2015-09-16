#include "VSet.hh"
#include <cstdio>
#include <cstdlib>

//TODO use bitset underneath?

VSet::VSet()
{
}


VSet::VSet(const Graph &G)
{
	auto iterInfo = boost::vertices(G);
	//Just go through and set the bit for each vertex to 1
	for (auto iter = iterInfo.first; iter != iterInfo.second; iter++)
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
	for (auto iter = vec.begin(); iter != vec.end(); iter++)
	{
		//bitVec.set(*iter, true);
		bitVec[*iter] = true;
	}

}


void VSet::insert(Vertex v)
{
	bitVec[v] = true;

}

void VSet::erase(Vertex v)
{
	bitVec[v] = false;

}

bool VSet::contains(Vertex v) const
{
	return bitVec[v];

}

void VSet::members(std::vector<Vertex>& vec) const
{
	vec.resize(bitVec.count());

	int vecPos = 0;

	for (int i = 0; i < MAX_NUM_VERTICES; i++)
	{
		//Check if the ith bit is set. If it is, add that to our vector
		if (bitVec[i])
		{
			//std::cout << "Setting membres pos " << vecPos << " to " << i << "\n";

			vec[vecPos] = ((Vertex)i);
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

//TODO keep counter
bool VSet::empty() const
{
	return bitVec.none();

}

int VSet::size() const
{
	return bitVec.count();


}

unsigned long long VSet::asInt() const
{
	return bitVec.to_ullong();


}

Vertex VSet::first() const
{
	for (int i = 0; i < MAX_NUM_VERTICES; i++)
	{
		//Check if the ith bit is set. If it is, add that to our vector
		if (bitVec[i])
		{
			//std::cout << "Setting membres pos " << vecPos << " to " << i << "\n";

			return i;
		}

	}
}

Vertex VSet::firstNotContained(int numVerts) const
{
	for (int i = 0; i < numVerts; i++)
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

void VSet::addAll(const VSet& that)
{
	this->bitVec |= that.bitVec;
}

