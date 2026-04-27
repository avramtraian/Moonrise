/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Assertions.h>
#include <AND/Types.h>

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

template<typename T>
requires(is_integer<T>)
NODISCARD ALWAYS_INLINE constexpr bool is_power_of_two(T value)
{
    return value > 0 && ((value & (value - 1)) == 0);
}

template<typename T>
requires(is_integer<T>)
NODISCARD ALWAYS_INLINE constexpr T round_up_to_power_of_two(T value, T power_of_two)
{
    if (!is_power_of_two(power_of_two))
        PANIC("Trying to invoke 'round_up_to_power_of_two' with a non-power-of-two value!");

    T mask = ~(power_of_two - 1);
    return (value + mask) & mask;
}

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::clamp;
using AND::divide_int_up;
using AND::is_power_of_two;
using AND::max;
using AND::min;
using AND::round_up_to_power_of_two;
#endif
