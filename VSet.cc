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
		currentSize++;

	}

}

VSet::VSet(const VSet &S)
{
	currentSize = S.currentSize;
	bitVec = S.bitVec;
	//memcpy(bitVec, S.bitVec, MAX_NUM_VERTICES);


}

VSet::VSet(const std::vector<Vertex>& vec)
{
	for (auto iter = vec.begin(); iter != vec.end(); iter++)
	{
		//bitVec.set(*iter, true);
		bitVec[*iter] = true;
		currentSize++;
	}

}


void VSet::insert(Vertex v)
{
	//TODO too slow?
	if (!bitVec[v])
	{
		currentSize++;
	}
	bitVec[v] = true;

}

void VSet::erase(Vertex v)
{
	if (bitVec[v])
	{
		currentSize--;
	}
	bitVec[v] = false;

}

bool VSet::contains(Vertex v) const
{
	return bitVec[v];

}

void VSet::members(std::vector<Vertex>& vec) const
{
	vec.resize(currentSize);

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
	return currentSize == 0;

}

int VSet::size() const
{
	return currentSize;


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
	auto newCount = this->currentSize + that.currentSize - (this->bitVec & that.bitVec).count();
	VSet ret;
	ret.bitVec = newBitVec;
	ret.currentSize = newCount;
	return ret;
}


