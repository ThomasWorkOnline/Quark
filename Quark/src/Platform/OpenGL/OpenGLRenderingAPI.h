#pragma once

#include "../../Quark/Renderer/RenderingAPI.h"

namespace Quark {

    class OpenGLRenderingAPI : public RenderingAPI
    {
    public:
        void Init() override;

        Version GetVersion() const override;
        void SetClearColor(const glm::vec4& rgba) override;
        void Clear() override;
        void SetCullFace(RenderCullFace face) override;
        void SetDepthFunction(RenderDepthFunction func) override;
        void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        void Draw(uint32_t offset, uint32_t count) override;
        void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
        void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0) override;
        void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t count) override;
        void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

        void SetLineThickness(float thickness) override;
        float GetLineThickness() const override;

        int32_t GetMaxTextureSlots() const override;
        int32_t GetMaxTextureSize() const override;
        int32_t GetMaxTextureLayers() const override;
        int32_t GetMaxUniformBufferBindings() const override;
        int32_t GetMaxVertexUniformBuffers() const override;
        int32_t GetMaxGeometryUniformBuffers() const override;
        int32_t GetMaxFragmentUniformBuffers() const override;

        const char* GetName() const override { return "OpenGL"; }
        std::string GetSpecification() const override;
    };
}
