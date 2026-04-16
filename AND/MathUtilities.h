/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

template<typename T>
requires(is_number<T>)
NODISCARD ALWAYS_INLINE constexpr T min(T a, T b)
{
    return a < b ? a : b;
}

template<typename T>
requires(is_number<T>)
NODISCARD ALWAYS_INLINE constexpr T max(T a, T b)
{
    return a > b ? a : b;
}

template<typename T>
requires(is_number<T>)
NODISCARD ALWAYS_INLINE constexpr T clamp(T value, T min_bound, T max_bound)
{
    return min(max_bound, max(min_bound, value));
}

} // namespace AND
