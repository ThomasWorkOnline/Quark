#pragma once

#include "Quark/Renderer/GraphicsAPI.h"

namespace Quark {

	class MetalGraphicsAPI final : public GraphicsAPI
	{
	public:
		virtual void Init() override;

		virtual Version GetVersion() const override { return Version{}; }
		virtual const HardwareConstraints& GetHardwareConstraints() const override { return m_Constraints; }

		virtual void SetClearColor(const glm::vec4& rgba) override;
		virtual void Clear() override;
		virtual void SetCullFace(RenderCullFace face) override {}
		virtual void SetDepthFunction(RenderDepthFunction func) override {}
		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override {}

		virtual void Draw(uint32_t offset, uint32_t count) override {}
		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override {}
		virtual void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0) override {}
		virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t count) override {}
		virtual void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override {}

		virtual void SetLineThickness(float thickness) override {}
		virtual float GetLineThickness() const override { return 0.0f; }

		virtual const char* GetName() const override { return "Metal"; }
		virtual std::string GetSpecification() const override { return std::string(); }

	private:
		HardwareConstraints m_Constraints{};
	};
}
