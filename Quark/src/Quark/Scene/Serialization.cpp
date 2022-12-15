#include "qkpch.h"
#include "Serialization.h"

namespace Quark {

	// Base program address to overcome ASLR
	static uint8_t s_ProgramHandle;

	template<>
	void Serializer::SerializeValue<std::string>(const std::string& string)
	{
		std::fwrite(string.data(), sizeof(std::string::value_type), string.size() + 1, m_OutFile);
	}

	template<>
	std::string Deserializer::DeserializeValue()
	{
		auto begin = std::ftell(m_InFile);
		std::string::value_type c;

		do {
			// Assume string is null terminated
			std::fread(&c, sizeof(c), 1, m_InFile);
		} while (c != 0);

		auto end = std::ftell(m_InFile);
		std::fseek(m_InFile, begin, SEEK_SET);

		auto strSize = end - begin - 1;

		std::string str;
		str.resize(strSize);
		std::fread(str.data(), sizeof(std::string::value_type), strSize, m_InFile);

		// Reset stream at end of string plus null-terminator
		std::fseek(m_InFile, end, SEEK_SET);

		return str;
	}

	template<>
	void Serializer::SerializeValue<std::uintptr_t>(const std::uintptr_t& fAddr)
	{
		auto offset = fAddr - reinterpret_cast<std::intptr_t>(&s_ProgramHandle);

		std::fwrite(&offset, sizeof(offset), 1, m_OutFile);
	}

	template<>
	std::uintptr_t Deserializer::DeserializeValue()
	{
		std::intptr_t offset;
		std::fread(&offset, sizeof(offset), 1, m_InFile);

		return reinterpret_cast<std::uintptr_t>(&s_ProgramHandle + offset);
	}
}
