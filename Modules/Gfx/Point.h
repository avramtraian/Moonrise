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
    T x { T(0) };
    T y { T(0) };
};

using IntPoint = Point<s32>;
using FloatPoint = Point<f32>;

} // namespace Gfx
