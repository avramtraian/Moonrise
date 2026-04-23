/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Types.h>

namespace Gfx {

template<typename T>
requires(is_number<T>)
struct Point {
public:
    static Point zero() { return { 0, 0 }; }

public:
    Point() = default;

    Point(T in_x, T in_y)
        : x(in_x)
        , y(in_y)
    {
    }

public:
    void move_by(Point delta)
    {
        x += delta.x;
        y += delta.y;
    }

    void move_by(T dx, T dy)
    {
        x += dx;
        y += dy;
    }

    void move_x_by(T dx) { x += dx; }
    void move_y_by(T dy) { y += dy; }

public:
    T x { T(0) };
    T y { T(0) };
};

using IntPoint = Point<s32>;
using FloatPoint = Point<f32>;

} // namespace Gfx
