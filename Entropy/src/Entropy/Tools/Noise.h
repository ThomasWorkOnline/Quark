#pragma once

#include "../Core/Core.h"

namespace Entropy {

	// Performant and accurate pseudo-random function from seed
	uint64_t Lehmer64(uint64_t seed);

	void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput);
}
