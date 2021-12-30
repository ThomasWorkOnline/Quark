#include "Noise.h"
#include <stdlib.h>

namespace Quark {

	uint64_t Lehmer64(uint64_t seed)
	{
		uint64_t nLehmer = seed;
		nLehmer += 0xe120fc15;
		uint64_t temp;
		temp = (uint64_t)nLehmer * 0x4a39b70d;
		uint64_t m1 = (temp >> 32) ^ temp;
		temp = (uint64_t)m1 * 0x12fad5c9;
		uint64_t m2 = (temp >> 32) ^ temp;
		return m2;
	}
}
