/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <Gfx/Color.h>

namespace Gfx {

Color Color::from_rgb(float r, float g, float b)
{
    Color result;
    result.m_format = ColorFormat::RGB;
    result.m_value.as_rgb.r = r;
    result.m_value.as_rgb.g = g;
    result.m_value.as_rgb.b = b;
    result.m_value.as_rgb.a = 1.0F;
    return result;
}

Color Color::from_rgba(float r, float g, float b, float a)
{
    Color result;
    result.m_format = ColorFormat::RGB;
    result.m_value.as_rgb.r = r;
    result.m_value.as_rgb.g = g;
    result.m_value.as_rgb.b = b;
    result.m_value.as_rgb.a = a;
    return result;
}

u32 Color::pack_to_rgba() const
{
    struct PackedRGBA {
        u8 r;
        u8 g;
        u8 b;
        u8 a;
    };

    PackedRGBA packed;
    packed.r = static_cast<u8>(m_value.as_rgb.r * 255.0F);
    packed.g = static_cast<u8>(m_value.as_rgb.g * 255.0F);
    packed.b = static_cast<u8>(m_value.as_rgb.b * 255.0F);
    packed.a = static_cast<u8>(m_value.as_rgb.a * 255.0F);
    return *reinterpret_cast<u32*>(&packed);
}

u32 Color::pack_to_bgra() const
{
    struct PackedBGRA {
        u8 b;
        u8 g;
        u8 r;
        u8 a;
    };

    PackedBGRA packed;
    packed.r = static_cast<u8>(m_value.as_rgb.r * 255.0F);
    packed.g = static_cast<u8>(m_value.as_rgb.g * 255.0F);
    packed.b = static_cast<u8>(m_value.as_rgb.b * 255.0F);
    packed.a = static_cast<u8>(m_value.as_rgb.a * 255.0F);
    return *reinterpret_cast<u32*>(&packed);
}

} // namespace Gfx
