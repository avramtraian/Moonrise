/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AT/Defines.h>
#include <MoonRise/GUI/Native/NativeWindow.h>

#if AT_PLATFORM_WINDOWS
    #include <MoonRise/GUI/Native/Windows/WindowsNativeWindow.h>
#endif // Any platform.

namespace MoonRise::GUI {

OwnPtr<NativeWindow> NativeWindow::instantiate()
{
#if AT_PLATFORM_WINDOWS
    WindowsNativeWindow* raw_native_windows_window = new WindowsNativeWindow();
    auto native_windows_window = adopt_own(raw_native_windows_window);
    return move(native_windows_window.as<NativeWindow>());
#endif // Any platform.
}

} // namespace MoonRise::GUI