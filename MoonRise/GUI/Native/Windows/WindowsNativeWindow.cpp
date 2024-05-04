/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */
#include "MoonRise/GUI/Native/Windows/WindowsNativeWindow.h"
#include "MoonRise/GUI/Application/Application.h"

namespace MoonRise::GUI {

static constexpr const char* s_window_class_name = "MoonRiseWindowClass";
static void register_window_class()
{
    static bool s_persistent_is_window_class_registered = false;
    if (s_persistent_is_window_class_registered)
        return;
    s_persistent_is_window_class_registered = true;

    WNDCLASSA window_class = {};
    window_class.hInstance = GetModuleHandleA(nullptr);
    window_class.lpfnWndProc = WindowsNativeWindow::window_procedure;
    window_class.lpszClassName = s_window_class_name;
    RegisterClassA(&window_class);
}

static ErrorOr<DWORD> get_window_style_flags(const WindowInfo& window_info)
{
    // A window can't be both minimized and maximized at the same time.
    if (window_info.start_maximized && window_info.start_minimized) {
        // TODO: Provide a better error code and message.
        return Error::Unknown;
    }

    DWORD style_flags = 0;
    if (window_info.start_minimized)
        style_flags |= WS_MAXIMIZE;
    if (window_info.start_minimized)
        style_flags |= WS_MINIMIZE;

    if (!window_info.start_in_fullscreen)
        style_flags |= WS_OVERLAPPEDWINDOW;

    return style_flags;
}

ErrorOr<void> WindowsNativeWindow::initialize(const WindowInfo& window_info)
{
    AT_ASSERT(m_native_handle == nullptr);
    register_window_class();
    m_should_close = false;

    const char* window_title = window_info.title.byte_span_with_null_termination().as<const char>().elements();
    TRY_ASSIGN(const DWORD window_style_flags, get_window_style_flags(window_info));

    HWND window_handle = CreateWindowA(
        s_window_class_name,
        window_title,
        window_style_flags,
        window_info.position_x,
        window_info.position_y,
        window_info.width,
        window_info.height,
        nullptr,
        nullptr,
        GetModuleHandleA(nullptr),
        nullptr
    );

    if (!window_handle) {
        // TODO: Provide a better error code and message.
        return Error::Unknown;
    }

    m_native_handle = window_handle;
    ShowWindow(m_native_handle, SW_SHOW);
    return {};
}

#define GET_NATIVE_WINDOW_OR_BREAK(variable_name)                                \
    Window* window = g_application->find_window_by_native_handle(window_handle); \
    if (!window)                                                                 \
        break;                                                                   \
    WindowsNativeWindow& variable_name = static_cast<WindowsNativeWindow&>(window->native().get());

LRESULT WindowsNativeWindow::window_procedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    switch (message) {
        case WM_QUIT:
        case WM_CLOSE: {
            GET_NATIVE_WINDOW_OR_BREAK(native_window);
            native_window.m_should_close = true;
            // TODO: Propagate close request event.
            return 0;
        }

        case WM_SIZE: {
            GET_NATIVE_WINDOW_OR_BREAK(native_window);

            // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-size
            const u32 new_width = LOWORD(l_param);
            const u32 new_height = HIWORD(l_param);

            if (w_param == SIZE_MINIMIZED) {
                // NOTE: We don't dispatch a window size changed event when the window has been minimized.
                native_window.m_state_mode = WindowStateMode::Minimized;
            }
            else {
                native_window.m_state_mode =
                    (w_param == SIZE_MAXIMIZED) ? WindowStateMode::Maximized : WindowStateMode::Normal;

                if (native_window.m_client_area_width != new_width ||
                    native_window.m_client_area_height != new_height) {
                    native_window.m_client_area_width = new_width;
                    native_window.m_client_area_height = new_height;
                    // TODO: Propagate size changed event.
                }
            }

            return 0;
        }

        case WM_MOVE: {
            GET_NATIVE_WINDOW_OR_BREAK(native_window);

            // https://learn.microsoft.com/en-us/windows/win32/winmsg/wm-move
            const POINTS new_position = MAKEPOINTS(l_param);

            if (native_window.m_client_area_position_x != new_position.x ||
                native_window.m_client_area_position_y != new_position.y) {
                native_window.m_client_area_width = new_position.x;
                native_window.m_client_area_height = new_position.y;
                // TODO: Propagate position changed event.
            }

            return 0;
        }
    }

    return DefWindowProcA(window_handle, message, w_param, l_param);
}

} // namespace MoonRise::GUI