#pragma once

#include <cstdio>
#include <string>

namespace Quark {

	template<>
	inline void Serialize(const std::string& data, FILE* out)
	{
		const char* tagData = data.c_str();
		std::fwrite(tagData, 1, data.size() + 1, out);
	}

	template<>
	inline auto Deserialize(FILE* in) -> std::string
	{
		size_t begin = std::ftell(in);
		char c;

		do {
			// Assume string is null terminated
			std::fread(&c, sizeof(char), 1, in);
		} while (c != 0);

		size_t end = std::ftell(in);
		std::fseek(in, (long)begin, SEEK_SET);

		size_t strSize = end - begin - 1;

		std::string str;
		str.resize(strSize);
		std::fread(str.data(), sizeof(char), strSize, in);

		// Reset stream at end of string plus null-terminator
		std::fseek(in, (long)end, SEEK_SET);

		return str;
	}

	// Base program address to overcome ASLR
	static uint8_t s_ProgramHandle;

	template<>
	inline void Serialize(void* const& func, FILE* out)
	{
		auto fAddr = reinterpret_cast<intptr_t>(func);
		auto offset = fAddr - reinterpret_cast<intptr_t>(&s_ProgramHandle);

		std::fwrite(&offset, sizeof(offset), 1, out);
	}

	template<>
	inline auto Deserialize(FILE* in) -> intptr_t
	{
		intptr_t offset;
		std::fread(&offset, sizeof(offset), 1, in);

		return reinterpret_cast<intptr_t>(&s_ProgramHandle) + offset;
	}
}
