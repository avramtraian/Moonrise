/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/MathUtilities.h>
#include <AND/Types.h>

namespace Gfx {

template<typename T>
requires(is_number<T>)
struct Border {
public:
    using PointType = AddSign<T>;

public:
    static Border uniform(u32 thickness) { return Border { thickness, thickness, thickness, thickness }; }

public:
    Border() = default;

    Border(T left, T right, T bottom, T top)
        : m_left(left)
        , m_right(right)
        , m_bottom(bottom)
        , m_top(top)
    {
    }

public:
    T left() const { return m_left; }
    T right() const { return m_right; }
    T bottom() const { return m_bottom; }
    T top() const { return m_top; }

    T horizontal() const { return m_left + m_right; }
    T vertical() const { return m_bottom + m_top; }

    void clamp_horizontal(T max_horizontal)
    {
        m_left = clamp<T>(m_left, T(0), max_horizontal);
        m_right = clamp<T>(m_left, T(0), max_horizontal - m_left);
    }

    void clamp_vertical(T max_vertical)
    {
        m_bottom = clamp<T>(m_bottom, T(0), max_vertical);
        m_top = clamp<T>(m_top, T(0), max_vertical - m_bottom);
    }

private:
    T m_left { T(0) };
    T m_right { T(0) };
    T m_bottom { T(0) };
    T m_top { T(0) };
};

using IntBorder = Border<u32>;
using FloatBorder = Border<f32>;

} // namespace Gfx
