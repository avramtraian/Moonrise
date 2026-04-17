/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

#include <Gfx/Point.h>

namespace Gfx {

template<typename T>
requires(is_number<T>)
struct Rect {
public:
    NODISCARD ALWAYS_INLINE Point<T> offset() const { return m_offset; }
    NODISCARD ALWAYS_INLINE T offset_x() const { return m_offset.x; }
    NODISCARD ALWAYS_INLINE T offset_y() const { return m_offset.y; }

    NODISCARD ALWAYS_INLINE T size_x() const { return m_size_x; }
    NODISCARD ALWAYS_INLINE T size_y() const { return m_size_y; }

public:
    void set_offset(Point<T> offset) { m_offset = offset; }

    void set_offset(T x, T y)
    {
        m_offset.x = x;
        m_offset.y = y;
    }

    void set_offset_x(T x) { m_offset.x = x; }
    void set_offset_y(T y) { m_offset.y = y; }

    void set_size(T x, T y)
    {
        m_size_x = x;
        m_size_y = y;
    }

    void set_size_x(T x) { m_size_x = x; }
    void set_size_y(T y) { m_size_y = y; }

private:
    Point<T> m_offset;
    T m_size_x { T(0) };
    T m_size_y { T(0) };
};

using IntRect = Rect<s32>;
using FloatRect = Rect<f32>;

} // namespace Gfx
