/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Types.h>
#if AND_PLATFORM_OS_WINDOWS

#    include <GUI/Platform/WindowsWindowManager.h>

#    include "Gfx/Bitmap.h"

namespace GUI {

void WindowsWindowManager::initialize_impl()
{
}

void WindowsWindowManager::shutdown_impl()
{
}

void WindowsWindowManager::process_event_queue()
{
    MSG message;
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
}

NativeWindowHandle WindowsWindowManager::create_window()
{
    win32_register_class();
    HWND window_handle = CreateWindowW(
        reinterpret_cast<LPCWSTR>(m_window_class_name.value().code_units()),
        L"GUI Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    if (!window_handle)
        return invalid_native_window_handle;

    NativeWindowHandle window = ++m_last_generated_window;
    WindowStorage& window_storage = m_windows[window];
    window_storage.handle = window_handle;
    return window;
}

void WindowsWindowManager::destroy_window(NativeWindowHandle window)
{
    if (window == invalid_native_window_handle || !m_windows.contains(window))
        return;

    HWND handle = m_windows.get(window).value().handle;
    m_windows.remove(window);

    // It is important to call this function after we removed the window ID from the
    // internal map to avoid receiving further messages from it.
    DestroyWindow(handle);
}

void WindowsWindowManager::show_window(NativeWindowHandle window)
{
    if (window == invalid_native_window_handle)
        return;

    ASSERT(m_windows.contains(window));
    auto& window_storage = m_windows[window];

    ShowWindow(window_storage.handle, SW_SHOW);
}

bool WindowsWindowManager::window_should_close(NativeWindowHandle window)
{
    if (window == invalid_native_window_handle || !m_windows.contains(window))
        return false;
    return m_windows.get(window).value().should_close;
}

Optional<Gfx::IntSize> WindowsWindowManager::win32_get_client_size(HWND window_handle)
{
    RECT client_rect = {};
    if (!GetClientRect(window_handle, &client_rect))
        return {};

    LONG width = client_rect.right - client_rect.left;
    LONG height = client_rect.bottom - client_rect.top;
    return Gfx::IntSize(width, height);
}

Optional<Gfx::IntSize> WindowsWindowManager::get_window_size(NativeWindowHandle window)
{
    if (window == invalid_native_window_handle || !m_windows.contains(window))
        return {};

    auto const& window_storage = m_windows.get(window);
    HWND window_handle = window_storage.value().handle;
    return win32_get_client_size(window_handle);
}

Optional<Gfx::IntPoint> WindowsWindowManager::calculate_relative_position(NativeWindowHandle window, Gfx::IntPoint absolute_position)
{
    // Query the native handle.
    if (window == invalid_native_window_handle || !m_windows.contains(window))
        return {};
    auto const& window_storage = m_windows.get(window);
    HWND window_handle = window_storage.value().handle;

    // Convert the absolute position to the position relative to the client region origin, which
    // Windows considers to be the top-left corner of the client region.
    POINT relative_cursor = { absolute_position.x, absolute_position.y };
    if (!ScreenToClient(window_handle, &relative_cursor))
        return {};

    // Query the window client area height.
    auto maybe_size = win32_get_client_size(window_handle);
    if (!maybe_size.has_value())
        return {};
    Gfx::IntSize client_size = maybe_size.value();

    // Adjust the relative position to be relative to the bottom-left corner of the client region,
    // which is what the GUI library considers to be the origin.
    Gfx::IntPoint relative_position;
    relative_position.x = relative_cursor.x;
    relative_position.y = client_size.height() - relative_cursor.y;

    // Check that the mouse cursor is inside the window client region.
    Gfx::IntRect client_rect { Gfx::IntPoint::zero(), client_size };
    if (!client_rect.contains(relative_position))
        return {};

    return relative_position;
}

void WindowsWindowManager::set_window_title(NativeWindowHandle, String)
{
    ASSERT_NOT_REACHED;
}

void WindowsWindowManager::set_window_mode(NativeWindowHandle, WindowMode)
{
    ASSERT_NOT_REACHED;
}

void WindowsWindowManager::set_window_size(NativeWindowHandle, Gfx::IntSize)
{
    ASSERT_NOT_REACHED;
}

void WindowsWindowManager::present_back_buffer(NativeWindowHandle window, NonnullRefPtr<Gfx::Bitmap> const& back_buffer)
{
    if (window == invalid_native_window_handle || !m_windows.contains(window))
        return;

    auto& window_storage = m_windows.get(window).value();
    if (window_storage.device_context == nullptr) {
        window_storage.device_context = GetDC(window_storage.handle);
    }

    HDC device_context = window_storage.device_context;
    if (device_context == nullptr)
        return;

    RECT client_rect = {};
    if (!GetClientRect(window_storage.handle, &client_rect))
        return;
    LONG client_width = client_rect.right - client_rect.left;
    LONG client_height = client_rect.bottom - client_rect.top;

    BITMAPINFO bitmap_info = {};
    bitmap_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmap_info.bmiHeader.biWidth = back_buffer->size().width();
    bitmap_info.bmiHeader.biHeight = -static_cast<LONG>(back_buffer->size().height());
    bitmap_info.bmiHeader.biPlanes = 1;
    bitmap_info.bmiHeader.biBitCount = static_cast<WORD>(back_buffer->bytes_per_pixel() * 8);
    bitmap_info.bmiHeader.biCompression = BI_RGB;

    StretchDIBits(
        device_context,
        0, 0, client_width, client_height,
        0, 0, back_buffer->size().width(), back_buffer->size().height(),
        back_buffer->ro_byte_span().bytes(), &bitmap_info,
        DIB_RGB_COLORS, SRCCOPY);
}

NativeWindowHandle WindowsWindowManager::find_window_from_handle(HWND handle)
{
    NativeWindowHandle result = invalid_native_window_handle;
    m_windows.for_each([&](NativeWindowHandle window, WindowStorage const& window_storage) {
        if (window_storage.handle == handle) {
            result = window;
            return IterationDecision::Break;
        }
        return IterationDecision::Continue;
    });
    return result;
}

LRESULT CALLBACK WindowsWindowManager::win32_window_proc(HWND handle, UINT message, WPARAM w_param, LPARAM l_param)
{
    auto& manager = WindowsWindowManager::the();
    switch (message) {
    case WM_CLOSE:
    case WM_QUIT: {
        NativeWindowHandle window = manager.find_window_from_handle(handle);
        if (window != invalid_native_window_handle && manager.m_windows.contains(window)) {
            if (manager.on_window_requested_close.is_valid())
                manager.on_window_requested_close(window);
        }
        return 0;
    }

    case WM_SIZE: {
        NativeWindowHandle window = manager.find_window_from_handle(handle);
        if (window == invalid_native_timer_handle)
            break;

        WORD new_width = LOWORD(l_param);
        WORD new_height = HIWORD(l_param);

        if (manager.on_window_resized.is_valid())
            manager.on_window_resized(window, Gfx::IntSize { new_width, new_height });
        return 0;
    }

    case WM_MOUSEMOVE: {
        NativeWindowHandle window = manager.find_window_from_handle(handle);
        if (window == invalid_native_timer_handle)
            break;

        // NOTE: The window manager propagates mouse-moved events with absolute coordinates, so the arguments passed
        //       to the window procedure are not useful.
        POINT cursor_position;
        if (!GetCursorPos(&cursor_position))
            break;

        Gfx::IntPoint absolute_mouse_position { cursor_position.x, cursor_position.y };
        if (manager.on_mouse_moved.is_valid())
            manager.on_mouse_moved(absolute_mouse_position);
        return 0;
    }

    default:
        break;
    }

    return DefWindowProcW(handle, message, w_param, l_param);
}

void WindowsWindowManager::win32_register_class()
{
    if (m_window_class_name.has_value())
        return;

    m_window_class_name = UTF16_VIEW("MoonriseGUIWindowClass");
    WNDCLASSW window_class = {};
    window_class.hInstance = GetModuleHandle(nullptr);
    window_class.lpfnWndProc = win32_window_proc;
    window_class.lpszClassName = reinterpret_cast<LPCWSTR>(m_window_class_name.value().code_units());
    RegisterClassW(&window_class);
}

} // namespace GUI

#endif
