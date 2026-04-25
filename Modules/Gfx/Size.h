/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Forward.h>
#include <AND/Types.h>

namespace Gfx {

template<typename T>
requires(is_number<T>)
struct Size {
public:
    static Size zero() { return { 0, 0 }; }

public:
    Size() = default;

    Size(T in_x, T in_y)
        : x(in_x)
        , y(in_y)
    {
    }

public:
    NODISCARD ALWAYS_INLINE T width() const { return x; }
    NODISCARD ALWAYS_INLINE T height() const { return y; }

    bool is_degenerated() const
    {
        return (x == 0) || (y == 0);
    }

    bool operator==(Size const& other) const
    {
        return (x == other.x) && (y == other.y);
    }

    bool operator!=(Size const& other) const
    {
        return !(*this == other);
    }

public:
    T x { T(0) };
    T y { T(0) };
};

using IntSize = Size<u32>;
using FloatSize = Size<f32>;

void append_to_builder(StringBuilder&, Optional<StringView> const&, IntSize const&);
void append_to_builder(StringBuilder&, Optional<StringView> const&, FloatSize const&);

} // namespace Gfx
