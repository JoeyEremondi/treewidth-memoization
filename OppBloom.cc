#include "OppBloom.hh"



OppBloom::OppBloom(int arraySize, int min, int max)
{
	this->arraySizePerTW = arraySize;
	minTW = min;
	maxTW = max;
	hashArray = new VSet*[(maxTW - minTW) + 1];
	for (int i = this->minTW; i < this->maxTW + 1; i++)
	{
		hashArray[i - minTW] = new VSet[arraySize];
	}
}


OppBloom::~OppBloom()
{
	//Nothing to do but deallocate our arrays
	for (int i = this->minTW; i < this->maxTW + 1; i++)
	{
		delete[] hashArray[i - minTW];
	}
	delete[] hashArray;
}

bool OppBloom::forSureContains(int TW, VSet S)
{
	return hashArray[TW - minTW][nBitHash(S)] == S;
}

void OppBloom::insert(int TW, VSet S)
{
	hashArray[TW - minTW ][nBitHash(S)] = S;
}

int OppBloom::nBitHash(const VSet& S)
{
	return boost::hash_value(S.bitVec.m_bits) % arraySizePerTW;
}
