#include "qkpch.h"
#include "Filesystem.h"

#include <fstream>

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

					return result;
				}
			}

			QK_RUNTIME_ERROR("Could not open file '{0}'", filepath);
			return result;
		}
	}
}
