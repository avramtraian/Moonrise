/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Defines.h>

#if AT_PLATFORM_WINDOWS
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
    #include <Windows.h>
    #include <Windowsx.h>
#else
    #error Trying to include the Windows headers on a platform where they are not available!
#endif // AT_PLATFORM_WINDOWS
