#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/Buffer.h"

namespace Quark {

	struct CameraBufferData
	{
		Mat4f View = Mat4(1.0f);
		Mat4f Projection = Mat4(1.0f);
	};

	struct Vertex2D
	{
		glm::vec2 Position;
		glm::vec3 Color;
	};

	inline BufferLayout g_Vertex2DLayout = {
		{ ShaderDataType::Float2, "a_Position" },
		{ ShaderDataType::Float3, "a_Color"    }
	};

	class RenderPipeline
	{
	public:
		virtual ~RenderPipeline() = default;

		virtual void Begin(const Mat4f& cameraProjection, const Mat4f& cameraView) = 0;
		virtual void End() = 0;

		virtual void BeginRenderPass() = 0;
		virtual void EndRenderPass() = 0;
		virtual void Submit() = 0;

		virtual void OnViewportResized(uint32_t viewportWidth, uint32_t viewportHeight) = 0;

		static Scope<RenderPipeline> Create();
	};
}
