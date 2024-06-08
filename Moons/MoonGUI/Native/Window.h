/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Defines.h>

#if AT_PLATFORM_WINDOWS
    #include <MoonGUI/Native/Windows/WindowsWindow.h>
#endif // AT_PLATFORM_WINDOWS

namespace GUI::Native {

#if AT_PLATFORM_WINDOWS
// Declared in WindowsWindow.h
using Window = WindowsWindow;
#endif // AT_PLATFORM_WINDOWS

} // namespace GUI::Native
