/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/MathUtilities.h>
#include <Gfx/Border.h>
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

    static Rect without_border(Rect rect, Border<SizeType> border)
    {
        border.clamp_horizontal(rect.width());
        border.clamp_vertical(rect.height());

        Rect result;
        result.m_offset.x = rect.m_offset.x + border.left();
        result.m_offset.y = rect.m_offset.y + border.bottom();
        result.m_size.x = rect.m_size.x - border.horizontal();
        result.m_size.y = rect.m_size.y - border.vertical();
        return result;
    }

    static Rect with_border(Rect rect, Border<SizeType> border)
    {
        Rect result;
        result.m_offset.x = rect.m_offset.x - border.left();
        result.m_offset.y = rect.m_offset.y - border.bottom();
        result.m_size.x = rect.m_size.x + border.horizontal();
        result.m_size.y = rect.m_size.y + border.vertical();
        return result;
    }

public:
    ALWAYS_INLINE Rect()
        : m_offset({ 0, 0 })
        , m_size({ 0, 0 })
    {
    }

    ALWAYS_INLINE Rect(Point<T> offset, Size<SizeType> size)
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

    void move_by(Point<T> delta)
    {
        m_offset.x += delta.x;
        m_offset.y += delta.y;
    }

    void move_by(T dx, T dy)
    {
        m_offset.x += dx;
        m_offset.y += dy;
    }

    void move_x_by(T dx) { m_offset.x += dx; }
    void move_y_by(T dy) { m_offset.y += dy; }

    void center_in(Point<T> center_offset)
    {
        m_offset.x = center_offset.x - width() / 2;
        m_offset.y = center_offset.y - height() / 2;
    }

    void center_in(T center_x, T center_y)
    {
        m_offset.x = center_x - width() / 2;
        m_offset.y = center_y - height() / 2;
    }

    void center_x_in(T center_x)
    {
        m_offset.x = center_x - width() / 2;
    }

    void center_y_in(T center_y)
    {
        m_offset.y = center_y - height() / 2;
    }

    void push_left_edge_by(T dx)
    {
        dx = min<T>(dx, width());
        m_offset.x += dx;
        m_size.x -= dx;
    }

    void push_right_edge_by(T dx)
    {
        dx = max<T>(dx, -width());
        m_size.x += dx;
    }

    void push_bottom_edge_by(T dy)
    {
        dy = min<T>(dy, height());
        m_offset.y += dy;
        m_size.y -= dy;
    }

    void push_top_edge_by(T dy)
    {
        dy = max<T>(dy, -height());
        m_size.y += dy;
    }

public:
    Rect left_border(Border<SizeType> border) const
    {
        border.clamp_horizontal(width());
        border.clamp_vertical(height());

        Rect result;
        result.set_offset(m_offset.x, m_offset.y + border.bottom());
        result.set_size(border.left(), height() - border.vertical());
        return result;
    }

    Rect right_border(Border<SizeType> border) const
    {
        border.clamp_horizontal(width());
        border.clamp_vertical(height());

        Rect result;
        result.set_offset(m_offset.x + width() - border.right(), m_offset.y + border.bottom());
        result.set_size(border.right(), height() - border.vertical());
        return result;
    }

    Rect bottom_border(Border<SizeType> border) const
    {
        border.clamp_horizontal(width());
        border.clamp_vertical(height());

        Rect result;
        result.set_offset(m_offset.x, m_offset.y);
        result.set_size(width(), border.bottom());
        return result;
    }

    Rect top_border(Border<SizeType> border) const
    {
        border.clamp_horizontal(width());
        border.clamp_vertical(height());

        Rect result;
        result.set_offset(m_offset.x, m_offset.y + height() - border.top());
        result.set_size(width(), border.top());
        return result;
    }

public:
    bool is_degenerated() const
    {
        return m_size.is_degenerated();
    }

    bool contains(Point<T> point) const
    {
        bool result_x = m_offset.x <= point.x && point.x < m_offset.x + static_cast<T>(m_size.width());
        bool result_y = m_offset.y <= point.y && point.y < m_offset.y + static_cast<T>(m_size.height());
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

    Rect without_border(Border<SizeType> border) const { return without_border(*this, border); }
    Rect with_border(Border<SizeType> border) const { return with_border(*this, border); }

private:
    Point<T> m_offset;
    Size<SizeType> m_size;
};

using IntRect = Rect<s32>;
using FloatRect = Rect<f32>;

} // namespace Gfx
