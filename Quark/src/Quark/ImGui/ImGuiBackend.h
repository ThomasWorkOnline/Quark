#pragma once

struct ImDrawData;

namespace Quark {

	struct ImGuiBackend
	{
		void(*Init)(void* windowHandle)    = nullptr;
		void(*Shutdown)()                  = nullptr;
		void(*NewFrame)()                  = nullptr;
		void(*RenderDrawData)(ImDrawData*) = nullptr;
	};
}
