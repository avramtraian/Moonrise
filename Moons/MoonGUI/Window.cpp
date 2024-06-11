/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <MoonGUI/Window.h>

namespace GUI {

ErrorOr<void> Window::construct(u32 window_width, u32 window_height, Badge<Application>)
{
    Native::Window::Info native_window_info = {};
    native_window_info.client_width = window_width;
    native_window_info.client_height = window_height;

    TRY(m_native_window.initialize(native_window_info));
    return {};
}

} // namespace GUI
