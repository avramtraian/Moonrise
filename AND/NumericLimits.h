/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

template<typename T>
class NumericLimits {
public:
    NODISCARD ALWAYS_INLINE static constexpr T min()
    {
        static_assert(!"You must specialize NumericLimits<T> to use NumericLimits<T>::min()");
        return T {};
    }

    NODISCARD ALWAYS_INLINE static constexpr T max()
    {
        static_assert(!"You must specialize NumericLimits<T> to use NumericLimits<T>::max()");
        return T {};
    }
};

template<typename T>
requires(is_unsigned_integer<T>)
class NumericLimits<T> {
public:
    NODISCARD ALWAYS_INLINE static constexpr T min() noexcept { return 0; }
    NODISCARD ALWAYS_INLINE static constexpr T max() noexcept { return static_cast<T>(-1); }
};

template<typename T>
requires(is_signed_integer<T>)
class NumericLimits<T> {
public:
    NODISCARD ALWAYS_INLINE static constexpr T min() noexcept { return static_cast<T>(static_cast<T>(1) << (8 * sizeof(T) - 1)); }
    NODISCARD ALWAYS_INLINE static constexpr T max() noexcept { return ~NumericLimits::min(); }
};

#define AND_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE(type, min_value, max_value)          \
    template<>                                                                             \
    class NumericLimits<type> {                                                            \
    public:                                                                                \
        NODISCARD ALWAYS_INLINE static constexpr type min() noexcept { return min_value; } \
        NODISCARD ALWAYS_INLINE static constexpr type max() noexcept { return max_value; } \
    }

#undef AND_DECLARE_NUMERIC_LIMITS_FOR_PRIMITIVE_TYPE

} // namespace AND
