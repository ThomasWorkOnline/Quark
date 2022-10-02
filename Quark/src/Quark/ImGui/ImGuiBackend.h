#pragma once

#include <imgui.h>

namespace Quark {

	class ImGuiBackend
	{
	public:
		virtual ~ImGuiBackend() = default;

		virtual void Init(void* windowHandle) = 0;
		virtual void NewFrame() = 0;
		virtual void RenderDrawData(ImDrawData* drawData) = 0;
	};
}
