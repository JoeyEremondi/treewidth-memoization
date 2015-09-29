#ifndef __OPP__BLOOM__H__
#define __OPP__BLOOM__H__

#include "VSet.hh"

class OppBloom
{
public:
	OppBloom(int arraySize, int min, int max);
	~OppBloom();
	bool forSureContains(int TW, VSet S);
	void insert(int TW, VSet S);
	int nBitHash(const VSet& S);
	
private:
	VSet** hashArray;
	int maxTW;
	int minTW;
	int arraySizePerTW;
};
#endif
