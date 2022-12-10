#include "qkpch.h"
#include "Filesystem.h"

#include <fstream>

namespace Quark {

	std::string Filesystem::ReadTextFile(std::string_view filepath)
	{
		std::string result;
		std::ifstream in(filepath.data(), std::ios::ate | std::ios::in | std::ios::binary);
		if (in)
		{
			size_t size = in.tellg();
			if (size != -1)
			{
				result.resize(size);
				in.seekg(0, std::ios::beg);
				in.read(result.data(), size);
				in.close();

				return result;
			}
		}

		ThrowRuntimeError("Could not open file '{0}'", filepath);
		return result;
	}

	std::vector<uint32_t> Filesystem::ReadSpirvFile(std::string_view filepath)
	{
		std::vector<uint32_t> result;
		std::ifstream in(filepath.data(), std::ios::ate | std::ios::in | std::ios::binary);
		if (in)
		{
			size_t size = in.tellg();
			Verify(size % sizeof(uint32_t) == 0, "Invalid byte alignment for file: '{0}' (SPIR-V requires 4-bytes words)", filepath);

			if (size != -1)
			{
				result.resize(size / sizeof(uint32_t));
				in.seekg(0, std::ios::beg);
				in.read((char*)result.data(), size);
				in.close();

				return result;
			}
		}

		ThrowRuntimeError("Could not open file '{0}'", filepath);
		return result;
	}
}
