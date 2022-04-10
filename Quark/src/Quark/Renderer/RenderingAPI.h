#pragma once

#include "Quark/Core/Core.h"

#include "RenderModes.h"
#include "VertexArray.h"

namespace Quark {

	class RenderingAPI
	{
	public:
		// Supported APIs
		enum class API
		{
			None = 0, OpenGL = 1
		};

		struct Version
		{
			int Major;
			int Minor;
		};

		virtual ~RenderingAPI() = default;
		virtual void Init() = 0;
		virtual Version GetVersion() const = 0;

		virtual void SetClearColor(const glm::vec4& rgba) = 0;
		virtual void Clear() = 0;
		virtual void SetCullFace(RenderCullFace face) = 0;
		virtual void SetDepthFunction(RenderDepthFunction func) = 0;
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void Draw(uint32_t offset, uint32_t count) = 0;
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0) = 0;
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t count) = 0;
		virtual void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

		virtual void SetLineThickness(float thickness) = 0;
		virtual float GetLineThickness() const = 0;

		virtual const char* GetName() const = 0;
		virtual std::string GetSpecification() const = 0;

		virtual int32_t GetMaxTextureSlots() const = 0;
		virtual int32_t GetMaxTextureSize() const = 0;
		virtual int32_t GetMaxTextureLayers() const = 0;
		virtual int32_t GetMaxUniformBufferBindings() const = 0;
		virtual int32_t GetMaxVertexUniformBuffers() const = 0;
		virtual int32_t GetMaxGeometryUniformBuffers() const = 0;
		virtual int32_t GetMaxFragmentUniformBuffers() const = 0;

		static API GetAPI() { return s_API; }
		static std::thread::id GetThreadId() { return s_ThreadId; }

		static Scope<RenderingAPI> Create(API api);

	private:
		static API s_API;
		static std::thread::id s_ThreadId;
	};
}
