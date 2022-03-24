#include "Filesystem.h"

#include <fstream>
#include <sstream>

namespace Quark {

	namespace Filesystem {

		std::string ReadFile(std::string_view filepath)
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
					std::stringstream ss;
					ss << "Could not read from file '" << filepath << "'";
					QK_FATAL(ss.str());
				}
			}
			else
			{
				std::stringstream ss;
				ss << "Could not open file '" << filepath << "'";
				QK_FATAL(ss.str());
			}

			return result;
		}
	}
}
