#pragma once

#include "Core.h"

#include "../Renderer/RenderingAPI.h"

namespace Quark {

    enum ApplicationFlags
    {
        ApplicationNoFlags = 0,
        ShowApiInWindowTitle = BIT(0)
    };

    struct ApplicationOptions
    {
        uint32_t Width = 1280, Height = 720;
        std::string Title = "Quark Engine";
        ApplicationFlags Flags = ApplicationNoFlags;

        RenderingAPI::API Api = RenderingAPI::API::OpenGL;

        bool HasFlag(ApplicationFlags flag) const
        {
            return Flags & flag;
        }

        ApplicationOptions() = default;
        ApplicationOptions(
            uint32_t width, uint32_t height,
            const std::string& title,
            ApplicationFlags flags = ApplicationNoFlags,
            RenderingAPI::API api = RenderingAPI::API::OpenGL
        )
            : Width(width), Height(height),
            Title(title), Flags(flags), Api(api)
        {
        }
    };
}
