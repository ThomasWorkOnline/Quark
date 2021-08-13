#pragma once

#include "../Core/Core.h"

#include <random>

namespace Quark {

	// Performant and accurate pseudo-random function from seed
	uint64_t Lehmer64(uint64_t seed);

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

		static uint32_t GetDefaultSeed() { return std::mt19937::default_seed; }

	private:
		std::mt19937 m_NoiseEngine{};
	};

	class PerlinNoise
	{
	public:
		explicit PerlinNoise(uint32_t seed = std::default_random_engine::default_seed);

		void Reseed(uint32_t seed);

		float Noise1D(float x) const noexcept;
		float Noise2D(float x, float y) const noexcept;
		float Noise3D(float x, float y, float z) const noexcept;

		float AccumulatedOctaveNoise1D(float x, int32_t octaves) const noexcept;
		float AccumulatedOctaveNoise2D(float x, float y, int32_t octaves) const noexcept;
		float AccumulatedOctaveNoise3D(float x, float y, float z, int32_t octaves) const noexcept;

		float NormalizedOctaveNoise1D(float x, int32_t octaves) const noexcept;
		float NormalizedOctaveNoise2D(float x, float y, int32_t octaves) const noexcept;
		float NormalizedOctaveNoise3D(float x, float y, float z, int32_t octaves) const noexcept;

		float UnsignedNormalizedOctaveNoise1D(float x, int32_t octaves) const noexcept;
		float UnsignedNormalizedOctaveNoise2D(float x, float y, int32_t octaves) const noexcept;
		float UnsignedNormalizedOctaveNoise3D(float x, float y, float z, int32_t octaves) const noexcept;

	private:
		uint8_t p[512];

		static constexpr float Fade(float t) noexcept;
		static constexpr float Lerp(float t, float a, float b) noexcept;
		static constexpr float Grad(uint8_t hash, float x, float y, float z) noexcept;
		static constexpr float Weight(int32_t octaves) noexcept;
	};
}
