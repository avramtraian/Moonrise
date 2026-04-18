/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once
#include "AND/Types.h"

namespace Gfx {

enum class ColorFormat {
    Invalid = 0,
    RGB,
};

class Color {
public:
    static Color from_rgb(float, float, float);
    static Color from_rgba(float, float, float, float);

public:
    static Color white() { return from_rgb(1.0f, 1.0f, 1.0f); }
    static Color black() { return from_rgb(0.0f, 0.0f, 0.0f); }
    static Color transparent() { return from_rgba(0.0f, 0.0f, 0.0f, 0.0f); }
    static Color red() { return from_rgb(1.0f, 0.0f, 0.0f); }
    static Color green() { return from_rgb(0.0f, 1.0f, 0.0f); }
    static Color blue() { return from_rgb(0.0f, 0.0f, 1.0f); }
    static Color yellow() { return from_rgb(1.0f, 1.0f, 0.0f); }
    static Color magenta() { return from_rgb(1.0f, 0.0f, 1.0f); }
    static Color cyan() { return from_rgb(0.0f, 1.0f, 1.0f); }

public:
    Color()
        : m_format(ColorFormat::RGB)
    {
        m_value.as_rgb.r = 0.0f;
        m_value.as_rgb.g = 0.0f;
        m_value.as_rgb.b = 0.0f;
        m_value.as_rgb.a = 0.0f;
    }

    Color(Color const&) = default;
    Color& operator=(Color const&) = default;

    u32 pack_to_rgba() const;
    u32 pack_to_bgra() const;

private:
    struct RGB {
        float r;
        float g;
        float b;
        float a;
    };

    ColorFormat m_format;
    union {
        RGB as_rgb;
    } m_value;
};

} // namespace Gfx
