#include "qkpch.h"
#include "Clipboard.h"

#include <GLFW/glfw3.h>

namespace Quark {

	std::string_view Clipboard::GetString()
	{
		const char* value = glfwGetClipboardString(NULL);
		return value ? value : std::string_view();
	}

	void Clipboard::SetString(std::string_view string)
	{
		glfwSetClipboardString(NULL, string.data());
	}

}
