/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/MathUtilities.h>
#include <Gfx/Point.h>
#include <Gfx/Size.h>

namespace Gfx {

template<typename T>
requires(is_number<T>)
struct Rect {
public:
    using SizeType = RemoveSign<T>;

public:
    static Rect min_max(Point<T> min, Point<T> max)
    {
        if (min.x > max.x || min.y > max.y)
            return Rect();

        Rect result;
        result.m_offset.x = min.x;
        result.m_offset.y = min.y;
        result.m_size.x = max.x - min.x;
        result.m_size.y = max.y - min.y;
        return result;
    }

    static Rect offset_size(Point<T> offset, Size<T> size)
    {
        Rect result;
        result.m_offset = offset;
        result.m_size = size;
        return result;
    }

    static Rect intersect(Rect a, Rect b)
    {
        auto min_x = max<s32>(a.m_offset.x, b.m_offset.x);
        auto min_y = max<s32>(a.m_offset.y, b.m_offset.y);
        auto max_x = min<s32>(a.m_offset.x + a.m_size.x, b.m_offset.x + b.m_size.x);
        auto max_y = min<s32>(a.m_offset.y + a.m_size.y, b.m_offset.y + b.m_size.y);
        return Rect::min_max({ min_x, min_y }, { max_x, max_y });
    }

public:
    Rect() = default;

    Rect(Point<T> offset, Size<SizeType> size)
        : m_offset(offset)
        , m_size(size)
    {
    }

public:
    NODISCARD ALWAYS_INLINE Point<T> offset() const { return m_offset; }
    NODISCARD ALWAYS_INLINE T offset_x() const { return m_offset.x; }
    NODISCARD ALWAYS_INLINE T offset_y() const { return m_offset.y; }

    NODISCARD ALWAYS_INLINE Size<SizeType> size() const { return m_size; }
    NODISCARD ALWAYS_INLINE SizeType width() const { return m_size.x; }
    NODISCARD ALWAYS_INLINE SizeType height() const { return m_size.y; }

    NODISCARD ALWAYS_INLINE Point<T> min_point() const { return m_offset; }
    NODISCARD ALWAYS_INLINE Point<T> max_point() const { return Point<T>(offset_x() + width(), offset_y() + height()); }

public:
    void set_offset(Point<T> offset) { m_offset = offset; }
    void set_offset_x(T x) { m_offset.x = x; }
    void set_offset_y(T y) { m_offset.y = y; }

    void set_offset(T x, T y)
    {
        m_offset.x = x;
        m_offset.y = y;
    }

    void set_size(Size<SizeType> size) { m_size = size; }
    void set_width(SizeType width) { m_size.x = width; }
    void set_height(SizeType height) { m_size.y = height; }

    void set_size(SizeType width, SizeType height)
    {
        m_size.x = width;
        m_size.y = height;
    }

public:
    bool is_degenerated() const
    {
        return m_size.is_degenerated();
    }

    bool contains(Point<T> point) const
    {
        bool result_x = m_offset.x <= point.x && point.x < m_offset.x + m_size.width;
        bool result_y = m_offset.y <= point.y && point.y < m_offset.y + m_size.height;
        return result_x && result_y;
    }

    bool overlaps(Rect other) const
    {
        auto this_min = min_point();
        auto this_max = max_point();
        auto other_min = other.min_point();
        auto other_max = other.max_point();
        return contains(other_min) || contains(other_max) || other.contains(this_min) || other.contains(this_max);
    }

private:
    Point<T> m_offset;
    Size<SizeType> m_size;
};

using IntRect = Rect<s32>;
using FloatRect = Rect<f32>;

} // namespace Gfx
