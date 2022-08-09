#pragma once

#include <random>

namespace Quark {

	template<typename T>
	class Random
	{
	public:
		static constexpr uint32_t DefaultSeed = std::mt19937::default_seed;

		explicit Random(uint32_t seed = std::mt19937::default_seed)
			: m_NoiseEngine(seed)
		{
		}

		inline void Seed(uint32_t seed)
		{
			m_NoiseEngine.seed(seed);
		}

		inline void Discard(uint32_t offset)
		{
			m_NoiseEngine.discard(offset);
		}

		inline T Next()
		{
			static_assert(false, "No default template specialization for type T");
		}

	private:
		std::mt19937 m_NoiseEngine;
	};

	template<>
	inline float Random<float>::Next()
	{
		// Return value normalized in range [0, 1]
		return m_NoiseEngine() / static_cast<float>(std::numeric_limits<uint32_t>::max());
	}

	template<>
	inline double Random<double>::Next()
	{
		// Return value normalized in range [0, 1]
		return m_NoiseEngine() / static_cast<double>(std::numeric_limits<uint32_t>::max());
	}

	template<>
	inline uint32_t Random<uint32_t>::Next()
	{
		// Return value unnormalized in range [0, UINT32_MAX]
		return m_NoiseEngine();
	}
}
