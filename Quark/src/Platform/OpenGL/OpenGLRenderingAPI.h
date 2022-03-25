#pragma once

#include "../../Quark/Renderer/RenderingAPI.h"

namespace Quark {

    class OpenGLRenderingAPI : public RenderingAPI
    {
    public:
        virtual void Init() override;

        virtual Version GetVersion() const override;
        virtual void SetClearColor(const glm::vec4& rgba) override;
        virtual void Clear() override;
        virtual void SetCullFace(RenderCullFace face) override;
        virtual void SetDepthFunction(RenderDepthFunction func) override;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

        virtual void Draw(uint32_t offset, uint32_t count) override;
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
        virtual void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0) override;

        virtual void DrawLines(const Ref<VertexArray>& vertexArray, uint32_t count) override;
        virtual void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;

        virtual void SetLineThickness(float thickness) override;
        virtual float GetLineThickness() const override;

        virtual int32_t GetMaxTextureSlotsCount() const override;
        virtual int32_t GetMaxTextureSize() const override;
        virtual int32_t GetMaxTextureLayers() const override;

        virtual const char* GetName() const override { return "OpenGL"; }
        virtual std::string GetSpecification() const override;
    };
}
