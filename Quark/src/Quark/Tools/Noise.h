#pragma once

#include <random>

namespace Quark {

	template<typename T>
	class PerlinNoise
	{
	public:
		static_assert(std::is_floating_point_v<T>,
			"T must be of floating-point type");

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
		uint8_t m_P[512];

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
			m_P[i] = static_cast<uint8_t>(i);
		}

		std::shuffle(std::begin(m_P), std::begin(m_P) + 256, std::default_random_engine(seed));

		for (size_t i = 0; i < 256; ++i)
		{
			m_P[256 + i] = m_P[i];
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

		const int32_t A = m_P[X] + Y, AA = m_P[A] + Z, AB = m_P[A + 1] + Z;
		const int32_t B = m_P[X + 1] + Y, BA = m_P[B] + Z, BB = m_P[B + 1] + Z;

		return Lerp(w, Lerp(v, Lerp(u, Grad(m_P[AA], x, y, z),
			   Grad(m_P[BA], x - 1, y, z)),
			   Lerp(u, Grad(m_P[AB], x, y - 1, z),
			   Grad(m_P[BB], x - 1, y - 1, z))),
			   Lerp(v, Lerp(u, Grad(m_P[AA + 1], x, y, z - 1),
			   Grad(m_P[BA + 1], x - 1, y, z - 1)),
			   Lerp(u, Grad(m_P[AB + 1], x, y - 1, z - 1),
			   Grad(m_P[BB + 1], x - 1, y - 1, z - 1))));
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
