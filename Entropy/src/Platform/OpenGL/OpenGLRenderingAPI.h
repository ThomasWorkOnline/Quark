#pragma once

#include "../../Entropy/Renderer/RenderingAPI.h"

namespace Entropy {

    class OpenGLRenderingAPI : public RenderingAPI
    {
    public:
        virtual void Init() override;
        virtual void SetClearColor(const glm::vec4& rgba) override;
        virtual void Clear() override;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
        virtual void Draw(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
        virtual void DrawInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0);

        virtual int32_t GetTextureSlotsCount() const override;
        virtual std::string GetSpecification() const override;
    };
}
