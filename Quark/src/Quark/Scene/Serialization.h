#pragma once

#include <string>
#include <cstdio>
#include <type_traits>

namespace Quark {

	class Serializer
	{
	public:
		Serializer(FILE* outFile)
			: m_OutFile(outFile)
		{
		}

		template<typename T>
		inline void SerializeValue(const T& value)
		{
			if constexpr (!std::is_trivially_constructible_v<T>)
				static_assert(sizeof(T) == 0, "No serialize specialization was defined for type T.");

			std::fwrite(&value, sizeof(T), 1, m_OutFile);
		}

	private:
		FILE* m_OutFile = nullptr;
	};

	class Deserializer
	{
	public:
		Deserializer(FILE* inFile)
			: m_InFile(inFile)
		{
		}

		template<typename T>
		inline auto DeserializeValue() -> T
		{
			if constexpr (!std::is_trivially_constructible_v<T>)
				static_assert(sizeof(T) == 0, "No deserialize specialization was defined for type T.");

			T value;
			std::fread(&value, sizeof(T), 1, m_InFile);
			return value;
		}

	private:
		FILE* m_InFile;
	};

	template<>
	void Serializer::SerializeValue<std::string>(const std::string& string);

	template<>
	std::string Deserializer::DeserializeValue();

	template<>
	void Serializer::SerializeValue<std::uintptr_t>(const std::uintptr_t& fAddr);

	template<>
	std::uintptr_t Deserializer::DeserializeValue();
}
