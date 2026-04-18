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

template<typename T>
requires(is_integer<T>)
NODISCARD ALWAYS_INLINE constexpr T divide_int_up(T numerator, T denominator)
{
    return (numerator + denominator - 1) / denominator;
}

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::clamp;
using AND::divide_int_up;
using AND::max;
using AND::min;
#endif
