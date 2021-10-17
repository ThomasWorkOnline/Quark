#pragma once

#include "../Core/Core.h"

#include "DeferredObjectDeleter.h"

#include "VertexArray.h"

// Implementations are done in the corresponding platform
namespace Quark {

    enum class RenderCullFace
    {
        None,
        Front,
        Back,
        FrontAndBack,

        Default = Back
    };

    enum class RenderDepthFunction
    {
        Never,
        Always,
        NotEqual,
        Less,
        LessEqual,
        Greater,
        GreaterEqual,

        Default = Less
    };

    class RenderingAPI
    {
    public:
        // Supported APIs
        enum class API
		{
            None = 0, OpenGL = 1
		};

        virtual ~RenderingAPI() = default;

        virtual void Init() = 0;
        virtual void SetClearColor(const glm::vec4& rgba) = 0;
        virtual void Clear() = 0;
        virtual void SetCullFace(RenderCullFace face) = 0;
        virtual void SetDepthFunction(RenderDepthFunction func) = 0;
        virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
        virtual void Draw(uint32_t offset, uint32_t count) = 0;
        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
        virtual void DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount = 0) = 0;
        virtual void DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;

        virtual int32_t GetMaxTextureSlotsCount() const = 0;
        virtual int32_t GetMaxTextureSize() const = 0;
        virtual int32_t GetMaxTextureLayers() const = 0;

        virtual std::string GetSpecification() const = 0;

        static API GetAPI() { return s_API; }
        static const char* GetName();

        static Scope<RenderingAPI> Create();

    private:
        static API s_API;
    };
}
