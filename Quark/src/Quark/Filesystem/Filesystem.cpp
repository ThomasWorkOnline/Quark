#include "qkpch.h"
#include "Filesystem.h"

#include <fstream>
#include <filesystem>

namespace Quark {

	namespace Filesystem {

		std::string ReadTextFile(std::string_view filepath)
		{
			std::string result;
			std::ifstream in(filepath.data(), std::ios::in | std::ios::binary);
			if (in)
			{
				in.seekg(0, std::ios::end);
				size_t size = in.tellg();
				if (size != -1)
				{
					result.resize(size);
					in.seekg(0, std::ios::beg);
					in.read(result.data(), size);
					in.close();
				}
				else
				{
					QK_CORE_FATAL("Could not open file '{0}'", filepath);
				}
			}
			else
			{
				QK_CORE_FATAL("Could not open file '{0}'", filepath);
			}

			return result;
		}
	}
}
