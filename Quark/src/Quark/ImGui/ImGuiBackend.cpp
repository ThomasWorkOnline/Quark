#include "qkpch.h"
#include <imgui.h>

namespace ImGuiBackend {

	inline void(*Init)(void*) = nullptr;
	inline void(*Shutdown)() = nullptr;
	inline void(*NewFrame)() = nullptr;
	inline void(*RenderDrawData)(ImDrawData*) = nullptr;
}
