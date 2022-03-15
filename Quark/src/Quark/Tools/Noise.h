#pragma once

#include "../Core/Core.h"

#include <random>

namespace Quark {

	template<typename T>
	class Random
	{
	public:
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
			return m_NoiseEngine() / static_cast<T>(std::numeric_limits<uint32_t>::max());
		}

		static constexpr uint32_t GetDefaultSeed() { return std::mt19937::default_seed; }

	private:
		std::mt19937 m_NoiseEngine;
	};

	template<typename T>
	class PerlinNoise
	{
	public:
		explicit PerlinNoise(uint32_t seed = std::default_random_engine::default_seed);
		void Reseed(uint32_t seed);

		T Noise1D(T x) const noexcept;
		T Noise2D(T x, T y) const noexcept;
		T Noise3D(T x, T y, T z) const noexcept;
		
		T AccumulatedOctaveNoise1D(T x, int32_t octaves) const noexcept;
		T AccumulatedOctaveNoise2D(T x, T y, int32_t octaves) const noexcept;
		T AccumulatedOctaveNoise3D(T x, T y, T z, int32_t octaves) const noexcept;

		T NormalizedOctaveNoise1D(T x, int32_t octaves) const noexcept;
		T NormalizedOctaveNoise2D(T x, T y, int32_t octaves) const noexcept;
		T NormalizedOctaveNoise3D(T x, T y, T z, int32_t octaves) const noexcept;

		T UnsignedNormalizedOctaveNoise1D(T x, int32_t octaves) const noexcept;
		T UnsignedNormalizedOctaveNoise2D(T x, T y, int32_t octaves) const noexcept;
		T UnsignedNormalizedOctaveNoise3D(T x, T y, T z, int32_t octaves) const noexcept;

	private:
		uint8_t p[512];

		static constexpr T Fade(T t) noexcept
		{
			return t * t * t * (t * (t * 6 - 15) + 10);
		}

		static constexpr T Lerp(T t, T a, T b) noexcept
		{
			return a + t * (b - a);
		}

		static constexpr T Grad(uint8_t hash, T x, T y, T z) noexcept
		{
			const uint8_t h = hash & 15;
			const T u = h < 8 ? x : y;
			const T v = h < 4 ? y : h == 12 || h == 14 ? x : z;
			return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
		}

		static constexpr T Weight(int32_t octaves) noexcept
		{
			T amp = 1;
			T value = 0;

			for (int32_t i = 0; i < octaves; ++i)
			{
				value += amp;
				amp /= 2;
			}

			return value;
		}
	};

	template<typename T>
	inline PerlinNoise<T>::PerlinNoise(uint32_t seed)
	{
		Reseed(seed);
	}

	template<typename T>
	inline void PerlinNoise<T>::Reseed(uint32_t seed)
	{
		for (size_t i = 0; i < 256; ++i)
		{
			p[i] = static_cast<uint8_t>(i);
		}

		std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

		for (size_t i = 0; i < 256; ++i)
		{
			p[256 + i] = p[i];
		}
	}

	template<typename T>
	T PerlinNoise<T>::Noise1D(T x) const noexcept
	{
		return Noise3D(x, 0, 0);
	}

	template<typename T>
	T PerlinNoise<T>::Noise2D(T x, T y) const noexcept
	{
		return Noise3D(x, y, 0);
	}

	template<typename T>
	T PerlinNoise<T>::Noise3D(T x, T y, T z) const noexcept
	{
		const int32_t X = static_cast<int32_t>(glm::floor(x)) & 255;
		const int32_t Y = static_cast<int32_t>(glm::floor(y)) & 255;
		const int32_t Z = static_cast<int32_t>(glm::floor(z)) & 255;

		x -= glm::floor(x);
		y -= glm::floor(y);
		z -= glm::floor(z);

		const T u = Fade(x);
		const T v = Fade(y);
		const T w = Fade(z);

		const int32_t A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
		const int32_t B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
			   Grad(p[BA], x - 1, y, z)),
			   Lerp(u, Grad(p[AB], x, y - 1, z),
			   Grad(p[BB], x - 1, y - 1, z))),
			   Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
			   Grad(p[BA + 1], x - 1, y, z - 1)),
			   Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
			   Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}

	template<typename T>
	T PerlinNoise<T>::AccumulatedOctaveNoise1D(T x, int32_t octaves) const noexcept
	{
		T result = 0;
		T amp = 1;

		for (int32_t i = 0; i < octaves; ++i)
		{
			result += Noise1D(x) * amp;
			x *= 2;
			amp /= 2;
		}

		return result; // unnormalized
	}

	template<typename T>
	T PerlinNoise<T>::AccumulatedOctaveNoise2D(T x, T y, int32_t octaves) const noexcept
	{
		T result = 0;
		T amp = 1;

		for (int32_t i = 0; i < octaves; ++i)
		{
			result += Noise2D(x, y) * amp;
			x *= 2;
			y *= 2;
			amp /= 2;
		}

		return result; // unnormalized
	}

	template<typename T>
	T PerlinNoise<T>::AccumulatedOctaveNoise3D(T x, T y, T z, int32_t octaves) const noexcept
	{
		T result = 0;
		T amp = 1;

		for (int32_t i = 0; i < octaves; ++i)
		{
			result += Noise3D(x, y, z) * amp;
			x *= 2;
			y *= 2;
			z *= 2;
			amp /= 2;
		}

		return result; // unnormalized
	}

	template<typename T>
	T PerlinNoise<T>::NormalizedOctaveNoise1D(T x, int32_t octaves) const noexcept
	{
		return AccumulatedOctaveNoise1D(x, octaves)
			/ Weight(octaves);
	}

	template<typename T>
	T PerlinNoise<T>::NormalizedOctaveNoise2D(T x, T y, int32_t octaves) const noexcept
	{
		return AccumulatedOctaveNoise2D(x, y, octaves)
			/ Weight(octaves);
	}

	template<typename T>
	T PerlinNoise<T>::NormalizedOctaveNoise3D(T x, T y, T z, int32_t octaves) const noexcept
	{
		return AccumulatedOctaveNoise3D(x, y, z, octaves)
			/ Weight(octaves);
	}

	template<typename T>
	T PerlinNoise<T>::UnsignedNormalizedOctaveNoise1D(T x, int32_t octaves) const noexcept
	{
		return glm::clamp<T>(AccumulatedOctaveNoise1D(x, octaves)
			* 0.5 + 0.5, 0, 1);
	}

	template<typename T>
	T PerlinNoise<T>::UnsignedNormalizedOctaveNoise2D(T x, T y, int32_t octaves) const noexcept
	{
		return glm::clamp<T>(AccumulatedOctaveNoise2D(x, y, octaves)
			* 0.5 + 0.5, 0, 1);
	}

	template<typename T>
	T PerlinNoise<T>::UnsignedNormalizedOctaveNoise3D(T x, T y, T z, int32_t octaves) const noexcept
	{
		return glm::clamp<T>(AccumulatedOctaveNoise3D(x, y, z, octaves)
			* 0.5 + 0.5, 0, 1);
	}
}
