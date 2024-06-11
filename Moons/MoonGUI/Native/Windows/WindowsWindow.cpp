/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <MoonCore/Log.h>
#include <MoonGUI/Native/Windows/WindowsWindow.h>

namespace GUI::Native {

static constexpr const char* DEFAULT_WINDOW_CLASS_NAME = "MoonriseWindowClass";
static bool s_is_default_window_class_registered { false };

static void win32_register_default_window_class()
{
    WNDCLASSA window_class = {};
    window_class.lpszClassName = DEFAULT_WINDOW_CLASS_NAME;
    window_class.hInstance = GetModuleHandle(nullptr);
    window_class.lpfnWndProc = WindowsWindow::window_procedure;

    RegisterClassA(&window_class);
}

WindowsWindow::WindowsWindow()
{
    if (!s_is_default_window_class_registered) {
        win32_register_default_window_class();
        s_is_default_window_class_registered = true;
    }
}

WindowsWindow::~WindowsWindow()
{
    if (!m_native_handle)
        return;
}

ErrorOr<void> WindowsWindow::initialize(const Info& info)
{
    if (m_native_handle) {
        errorln("The window has already been initialized!");
        return Error::AlreadyInitialized;
    }

    DWORD window_style_flags = WS_OVERLAPPEDWINDOW | WS_VISIBLE;
    if (info.start_maximized)
        window_style_flags |= WS_MAXIMIZE;

    m_native_handle = CreateWindowA(
        DEFAULT_WINDOW_CLASS_NAME,
        "Well hello there",
        window_style_flags,
        info.client_position_x,
        info.client_position_y,
        info.client_width,
        info.client_height,
        nullptr,
        nullptr,
        GetModuleHandle(nullptr),
        nullptr
    );

    if (!m_native_handle) {
        // TODO: Return a better error code, probably by querying the Win32 GetLastError() API.
        errorln("Failed to create the window!");
        return Error::Unknown;
    }

    return {};
}

void WindowsWindow::close()
{
    AT_ASSERT(m_should_close);

    if (m_native_handle) {
        DestroyWindow(static_cast<HWND>(m_native_handle));
        m_native_handle = nullptr;
    }
}

LRESULT WindowsWindow::window_procedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    // switch (message) {}

    return DefWindowProcA(window_handle, message, w_param, l_param);
}

} // namespace GUI::Native
