/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreDefines.h"
#include "AT/CoreTypes.h"

namespace AT {

//
// Invoked when an assertion is triggered. Its behaviour is dependent on the platform, but
// generally it will open a pop-up window that displays the assertion information, as well
// as printing an error log to the console.
// Currently, it is guaranteed that the program execution will hang immediately after calling
// this function, usually using a debugger trap/break.
//
AT_API void assertion_failed(const char* expression, const char* filename, const char* function, u32 line_number);

} // namespace AT

//
// Ensures that the given expression evaluates as true. Otherwise, assertion_failed() will be
// called and a debugger trap/break will be triggered, terminating the program execution.
// Should only be called when no recovery is possible, as it is considered a crash.
// No important code should run in the given expression, as the macro might be stripped
// out in specific build configurations.
//
#define AT_ASSERT(expression)                                               \
    if (!(expression)) {                                                    \
        ::AT::assertion_failed(#expression, AT_FILE, AT_FUNCTION, AT_LINE); \
        AT_DEBUGBREAK;                                                      \
    }

#if AT_CONFIGURATION_DEBUG
  // Exactly same behaviour as the normal AT_ASSERT(), but it is stripped out in the release builds.
    #define AT_ASSERT_DEBUG(expression)                                         \
        if (!(expression)) {                                                    \
            ::AT::assertion_failed(#expression, AT_FILE, AT_FUNCTION, AT_LINE); \
            AT_DEBUGBREAK;                                                      \
        }
#else
    #define AT_ASSERT_DEBUG(...) // Excluded from build.
#endif // AT_CONFIGURATION_DEBUG

constexpr bool ASSERTION_NOT_REACHED = false;
#define AT_ASSERT_NOT_REACHED       AT_ASSERT(ASSERTION_NOT_REACHED)
#define AT_ASSERT_NOT_REACHED_DEBUG AT_ASSERT_DEBUG(ASSERTION_NOT_REACHED)