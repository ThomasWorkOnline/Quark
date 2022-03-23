#pragma once

#include "Core.h"

namespace Quark {

    enum ApplicationFlag
    {
        ApplicationNoFlags = 0,
        ShowApiInWindowTitle = BIT(0)
    };

    struct ApplicationOptions
    {
        uint32_t Width = 1280, Height = 720;
        std::string Title = "Quark Engine";
        ApplicationFlag Flags = ApplicationNoFlags;

        bool HasFlag(ApplicationFlag flag) const
        {
            return Flags & flag;
        }

        ApplicationOptions() = default;
        ApplicationOptions(
            uint32_t width, uint32_t height,
            const std::string& title,
            ApplicationFlag flags = ApplicationNoFlags
        )
            : Width(width), Height(height),
            Title(title), Flags(flags)
        {
        }
    };
}
