#include "OpenGLRenderingAPI.h"

#include <glad/glad.h>

namespace Quark {

    static bool IsInIgnoreList(GLuint type)
    {
        switch (type)
        {
        case GL_DEBUG_TYPE_OTHER:    return true; // Buffer detailed info
        }

        return false;
    }

    static void OnError(
        GLenum source,
        GLenum type,
        GLuint id,
        GLenum severity,
        GLsizei length,
        const GLchar* message,
        const void* userParam)
    {
#       ifdef QK_DEBUG // Error printing only applies to debug mode

        if (IsInIgnoreList(type))
            return;

        fprintf(stderr, "OpenGL callback: %s(Type: 0x%x), [Severity: 0x%x]\n'%s'\n",
            (type == GL_DEBUG_TYPE_ERROR ? "[OpenGL ERROR]" : ""),
            type, severity, message);

        QK_ASSERT(type != GL_DEBUG_TYPE_ERROR, "OpenGL error");

#       endif
    }

    void OpenGLRenderingAPI::Init()
    {
        QK_TIME_SCOPE_DEBUG(OpenGLRenderingAPI::Init);

        glEnable(GL_DEBUG_OUTPUT);
        glDebugMessageCallback(OnError, 0);

        // Gamma correction
        glEnable(GL_FRAMEBUFFER_SRGB);

        // Alpha and Blending
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Face Culling
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);

        // Depth Testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        // Stencil Testing
        glEnable(GL_STENCIL_TEST);

        // Filtering
        glEnable(GL_MULTISAMPLE);
        glEnable(GL_LINE_SMOOTH); // <-- NOTE: this massively slows down line rendering

        // Extensions
        //glEnable(GL_TEXTURE_2D_ARRAY_EXT);

        // Experimental
        //glEnable(GL_PROGRAM_POINT_SIZE);
    }

    void OpenGLRenderingAPI::Clear()
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    }

    void OpenGLRenderingAPI::SetCullFace(RenderCullFace face)
    {
        // Front and back are reversed
        switch (face)
        {
        case RenderCullFace::None:
            glDisable(GL_CULL_FACE);
            break;
        case RenderCullFace::Front:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_BACK);
            break;
        case RenderCullFace::Back:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT);
            break;
        case RenderCullFace::FrontAndBack:
            glEnable(GL_CULL_FACE);
            glCullFace(GL_FRONT_AND_BACK);
            break;
        }
    }

    void OpenGLRenderingAPI::SetDepthFunction(RenderDepthFunction func)
    {
        switch (func)
        {
        case RenderDepthFunction::Never:
            glDepthFunc(GL_NEVER);
            break;
        case RenderDepthFunction::Always:
            glDepthFunc(GL_ALWAYS);
            break;
        case RenderDepthFunction::NotEqual:
            glDepthFunc(GL_NOTEQUAL);
            break;
        case RenderDepthFunction::Less:
            glDepthFunc(GL_LESS);
            break;
        case RenderDepthFunction::LessEqual:
            glDepthFunc(GL_LEQUAL);
            break;
        case RenderDepthFunction::Greater:
            glDepthFunc(GL_GREATER);
            break;
        case RenderDepthFunction::GreaterEqual:
            glDepthFunc(GL_GEQUAL);
            break;
        }
    }

    void OpenGLRenderingAPI::SetClearColor(const glm::vec4& rgba)
    {
        glClearColor(rgba.x, rgba.y, rgba.z, rgba.w);
    }

    void OpenGLRenderingAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        glViewport(x, y, width, height);
    }

    void OpenGLRenderingAPI::Draw(uint32_t offset, uint32_t count)
    {
        glDrawArrays(GL_TRIANGLES, offset, count);
    }

    void OpenGLRenderingAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer() ? vertexArray->GetIndexBuffer()->GetCount() : 0;
        glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRenderingAPI::DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, uint32_t repeatCount, uint32_t indexCount)
    {
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer() ? vertexArray->GetIndexBuffer()->GetCount() : 0;
        glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, repeatCount);
    }

    void OpenGLRenderingAPI::DrawLines(const Ref<VertexArray>& vertexArray, uint32_t count)
    {
        glDrawArrays(GL_LINES, 0, count);
    }

    void OpenGLRenderingAPI::DrawIndexedLines(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
    {
        uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer() ? vertexArray->GetIndexBuffer()->GetCount() : 0;
        glDrawElements(GL_LINES, count, GL_UNSIGNED_INT, nullptr);
    }

    void OpenGLRenderingAPI::SetLineThickness(float thickness)
    {
        glLineWidth(thickness);
    }

    float OpenGLRenderingAPI::GetLineThickness() const
    {
        float thickness;
        glGetFloatv(GL_LINE_WIDTH, &thickness);
        return thickness;
    }

    int32_t OpenGLRenderingAPI::GetMaxTextureSlotsCount() const
    {
        int32_t textureSlots;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureSlots);
        return textureSlots;
    }

    int32_t OpenGLRenderingAPI::GetMaxTextureSize() const
    {
        int32_t maxTextureSize;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
        return maxTextureSize;
    }

    int32_t OpenGLRenderingAPI::GetMaxTextureLayers() const
    {
        int32_t maxArrayTextureLayers;
        glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &maxArrayTextureLayers);
        return maxArrayTextureLayers;
    }

    std::string OpenGLRenderingAPI::GetSpecification() const
    {
        std::stringstream ss;
        ss << "OpenGL Info:\n";
        ss << "|\t" << glGetString(GL_VENDOR) << '\n';
        ss << "|\t" << glGetString(GL_RENDERER) << '\n';
        ss << "|\t" << glGetString(GL_VERSION) << '\n';
        ss << "|\tHardware texture slots available: " << GetMaxTextureSlotsCount() << '\n';
        return ss.str();
    }
}
