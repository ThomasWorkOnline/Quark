#pragma once

#include <string_view>

namespace Quark {

	struct Clipboard
	{
		static std::string_view GetString();
		static void SetString(std::string_view string);
	};

}
