/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreDefines.h"
#include "AT/CoreTypes.h"

namespace AT {

AT_API void assertion_failed(const char* expression, const char* filename, const char* function, u32 line_number);

} // namespace AT

#define AT_ASSERT(expression)                                               \
    if (!(expression)) {                                                    \
        ::AT::assertion_failed(#expression, AT_FILE, AT_FUNCTION, AT_LINE); \
        AT_DEBUGBREAK;                                                      \
    }

#if AT_CONFIGURATION_DEBUG

    #define AT_ASSERT_DEBUG(expression)                                         \
        if (!(expression)) {                                                    \
            ::AT::assertion_failed(#expression, AT_FILE, AT_FUNCTION, AT_LINE); \
            AT_DEBUGBREAK;                                                      \
        }

#else

    #define AT_ASSERT_DEBUG(...) // Excluded from build.

#endif // AT_CONFIGURATION_DEBUG