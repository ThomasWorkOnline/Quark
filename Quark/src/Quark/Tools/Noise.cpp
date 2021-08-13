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

	PerlinNoise::PerlinNoise(uint32_t seed)
	{
		Reseed(seed);
	}

	void PerlinNoise::Reseed(uint32_t seed)
	{
		for (size_t i = 0; i < 256; ++i)
		{
			p[i] = static_cast<std::uint8_t>(i);
		}

		std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

		for (size_t i = 0; i < 256; ++i)
		{
			p[256 + i] = p[i];
		}
	}

	float PerlinNoise::Noise1D(float x) const noexcept
	{
		return Noise3D(x, 0, 0);
	}

	float PerlinNoise::Noise2D(float x, float y) const noexcept
	{
		return Noise3D(x, y, 0);
	}

	float PerlinNoise::Noise3D(float x, float y, float z) const noexcept
	{
		const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
		const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
		const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;

		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);

		const float u = Fade(x);
		const float v = Fade(y);
		const float w = Fade(z);

		const std::int32_t A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
		const std::int32_t B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
			Grad(p[BA], x - 1, y, z)),
			Lerp(u, Grad(p[AB], x, y - 1, z),
				Grad(p[BB], x - 1, y - 1, z))),
			Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
				Grad(p[BA + 1], x - 1, y, z - 1)),
				Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
					Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}

	float PerlinNoise::AccumulatedOctaveNoise1D(float x, int32_t octaves) const noexcept
	{
		float result = 0;
		float amp = 1;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += Noise1D(x) * amp;
			x *= 2;
			amp /= 2;
		}

		return result; // unnormalized
	}

	float PerlinNoise::AccumulatedOctaveNoise2D(float x, float y, int32_t octaves) const noexcept
	{
		float result = 0;
		float amp = 1;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += Noise2D(x, y) * amp;
			x *= 2;
			y *= 2;
			amp /= 2;
		}

		return result; // unnormalized
	}

	float PerlinNoise::AccumulatedOctaveNoise3D(float x, float y, float z, int32_t octaves) const noexcept
	{
		float result = 0;
		float amp = 1;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += Noise3D(x, y, z) * amp;
			x *= 2;
			y *= 2;
			z *= 2;
			amp /= 2;
		}

		return result; // unnormalized
	}

	float PerlinNoise::NormalizedOctaveNoise1D(float x, int32_t octaves) const noexcept
	{
		return AccumulatedOctaveNoise1D(x, octaves)
			/ Weight(octaves);
	}

	float PerlinNoise::NormalizedOctaveNoise2D(float x, float y, int32_t octaves) const noexcept
	{
		return AccumulatedOctaveNoise2D(x, y, octaves)
			/ Weight(octaves);
	}

	float PerlinNoise::NormalizedOctaveNoise3D(float x, float y, float z, int32_t octaves) const noexcept
	{
		return AccumulatedOctaveNoise3D(x, y, z, octaves)
			/ Weight(octaves);
	}

	float PerlinNoise::UnsignedNormalizedOctaveNoise1D(float x, int32_t octaves) const noexcept
	{
		return std::clamp<float>(AccumulatedOctaveNoise1D(x, octaves)
			* 0.5 + 0.5, 0, 1);
	}

	float PerlinNoise::UnsignedNormalizedOctaveNoise2D(float x, float y, int32_t octaves) const noexcept
	{
		return std::clamp<float>(AccumulatedOctaveNoise2D(x, y, octaves)
			* 0.5 + 0.5, 0, 1);
	}

	float PerlinNoise::UnsignedNormalizedOctaveNoise3D(float x, float y, float z, int32_t octaves) const noexcept
	{
		return std::clamp<float>(AccumulatedOctaveNoise3D(x, y, z, octaves)
			* 0.5 + 0.5, 0, 1);
	}

	constexpr float PerlinNoise::Fade(float t) noexcept
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	constexpr float PerlinNoise::Lerp(float t, float a, float b) noexcept
	{
		return a + t * (b - a);
	}

	constexpr float PerlinNoise::Grad(uint8_t hash, float x, float y, float z) noexcept
	{
		const uint8_t h = hash & 15;
		const float u = h < 8 ? x : y;
		const float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	constexpr float PerlinNoise::Weight(int32_t octaves) noexcept
	{
		float amp = 1;
		float value = 0;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			value += amp;
			amp /= 2;
		}

		return value;
	}
}
