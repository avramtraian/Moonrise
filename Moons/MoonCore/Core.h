/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Defines.h"

#if AT_PLATFORM_WINDOWS
    #if AT_COMPILER_MSVC
        #define CORE_API_SPECIFIER_EXPORT __declspec(dllexport)
        #define CORE_API_SPECIFIER_IMPORT __declspec(dllimport)
    #else
        #define CORE_API_SPECIFIER_EXPORT
        #define CORE_API_SPECIFIER_IMPORT
    #endif // AT_COMPILER_MSVC
#else
    #define CORE_API_SPECIFIER_EXPORT
    #define CORE_API_SPECIFIER_IMPORT
#endif // AT_PLATFORM_WINDOWS

#ifdef MOON_CORE_EXPORT_API
    // Only defined when compiling the actual library.
    #define CORE_API CORE_API_SPECIFIER_EXPORT
#else
    #define CORE_API CORE_API_SPECIFIER_IMPORT
#endif // MOON_CORE_EXPORT_API
