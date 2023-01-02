#pragma once

#include <random>

namespace Quark {

	template<typename T>
	class Random
	{
	public:
		using NoiseEngine = std::mt19937;
		static constexpr uint32_t DefaultSeed = NoiseEngine::default_seed;

		explicit Random(uint32_t seed = DefaultSeed)
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
			static_assert(sizeof(T) == 0, "No default template specialization for type T");
		}

	private:
		NoiseEngine m_NoiseEngine;
	};

	template<>
	inline float Random<float>::Next()
	{
		// Return value normalized in range [0, 1]
		return m_NoiseEngine() / static_cast<float>(std::numeric_limits<NoiseEngine::result_type>::max());
	}

	template<>
	inline double Random<double>::Next()
	{
		// Return value normalized in range [0, 1]
		return m_NoiseEngine() / static_cast<double>(std::numeric_limits<NoiseEngine::result_type>::max());
	}

	template<>
	inline uint32_t Random<uint32_t>::Next()
	{
		// Return value unnormalized in range [0, UINT32_MAX]
		return m_NoiseEngine();
	}

	template<>
	inline int32_t Random<int32_t>::Next()
	{
		// Return value unnormalized in range [-INT32_MAX, INT32_MAX]
		return static_cast<int32_t>(m_NoiseEngine()) + std::numeric_limits<int32_t>::min();
	}

	template<>
	inline bool Random<bool>::Next()
	{
		// Returns a uniformly distributed boolean result
		return m_NoiseEngine() & 1;
	}
}
