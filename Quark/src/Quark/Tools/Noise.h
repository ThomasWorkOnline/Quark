#pragma once

#include "../Core/Core.h"

#include <random>

namespace Quark {

	// Performant and accurate pseudo-random function from seed
	uint64_t Lehmer64(uint64_t seed);

	void PerlinNoise2D(int nWidth, int nHeight, float* fSeed, int nOctaves, float fBias, float* fOutput);

	class Random
	{
	public:
		inline void Seed(uint32_t seed)
		{
			m_NoiseEngine.seed(seed);
		}

		inline void Discard(uint32_t offset)
		{
			m_NoiseEngine.discard(offset);
		}

		inline float Next()
		{
			return m_NoiseEngine() / static_cast<float>(std::numeric_limits<uint32_t>::max());
		}

		static uint32_t GetDefaultSeed() { return 5489U; }

	private:
		std::mt19937 m_NoiseEngine{};
	};
}
