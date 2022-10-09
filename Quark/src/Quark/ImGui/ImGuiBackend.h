#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/RenderPass.h"
#include "Quark/Renderer/CommandBuffer.h"

#include <imgui.h>

namespace Quark {

	class ImGuiBackend
	{
	public:
		virtual ~ImGuiBackend() = default;

		virtual void Init(RenderPass* renderPass, CommandBuffer* commandBuffer) = 0;
		virtual void NewFrame() = 0;
		virtual void RenderDrawData(CommandBuffer* commandBuffer, ImDrawData* drawData) = 0;

		static Scope<ImGuiBackend> Create(void* windowHandle, bool native = false);
	};
}
