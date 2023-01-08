#include "qkpch.h"
#include "Filesystem.h"

#include <fstream>

namespace Quark {

	std::string Filesystem::ReadTextFile(std::string_view filepath)
	{
		std::string result;
		std::ifstream in(filepath.data(), std::ios::ate | std::ios::in | std::ios::binary);

		QK_CORE_VERIFY(in, "Could not open file '{0}'", filepath);

		std::streampos size = in.tellg();
		if (size != -1)
		{
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read(result.data(), size);
			in.close();
		}

		return result;
	}

	std::vector<uint32_t> Filesystem::ReadSpirvFile(std::string_view filepath)
	{
		std::vector<uint32_t> result;
		std::ifstream in(filepath.data(), std::ios::ate | std::ios::in | std::ios::binary);

		QK_CORE_VERIFY(in, "Could not open file '{0}'", filepath);

		std::streampos size = in.tellg();
		QK_CORE_VERIFY(size % sizeof(uint32_t) == 0, "Invalid byte alignment for file: '{0}' (SPIR-V requires 4-bytes words)", filepath);

		if (size != -1)
		{
			result.resize(size / sizeof(uint32_t));
			in.seekg(0, std::ios::beg);
			in.read((char*)result.data(), size);
			in.close();
		}

		return result;
	}

	std::vector<uint8_t> Filesystem::ReadBinaryFile(std::string_view filepath)
	{
		std::vector<uint8_t> result;
		std::ifstream in(filepath.data(), std::ios::ate | std::ios::in | std::ios::binary);

		QK_CORE_VERIFY(in, "Could not open file '{0}'", filepath);

		std::streampos size = in.tellg();
		if (size != -1)
		{
			result.resize(size);
			in.seekg(0, std::ios::beg);
			in.read((char*)result.data(), size);
			in.close();
		}

		return result;
	}
}
