/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertion.h"
#include "AT/Defines.h"
#include "AT/Types.h"

namespace AT {

template<typename T>
struct TypeTraits {
    NODISCARD ALWAYS_INLINE static u64 hash(const T&)
    {
        AT_ASSERT_NOT_REACHED;
        return 0;
    }
};

template<typename T>
requires (is_integral<T>)
struct TypeTraits<T> {
    NODISCARD ALWAYS_INLINE static u64 hash(const T&) { return 0; }
};

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::TypeTraits;
#endif // AT_INCLUDE_GLOBALLY