#pragma once

#include "Quark/Core/Core.h"

#include <vector>

namespace Quark {

	namespace Filesystem {

		std::string           ReadTextFile(std::string_view filepath);
		std::vector<uint32_t> ReadSpirvFile(std::string_view filepath);
	}
}
