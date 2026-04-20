/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Types.h>
#if AND_PLATFORM_OS_WINDOWS

#    include <GUI/Platform/WindowsWindowManager.h>

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

WindowID WindowsWindowManager::create_window()
{
    win32_register_class();
    HWND window_handle = CreateWindowW(
        reinterpret_cast<LPCWSTR>(m_window_class_name.value().code_units()),
        L"GUI Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
        nullptr, nullptr, GetModuleHandle(nullptr), nullptr);
    if (!window_handle)
        return invalid_window_id;

    WindowID window_id = ++m_last_generated_window_id;
    WindowStorage& window_storage = m_windows[window_id];
    window_storage.handle = window_handle;
    return window_id;
}

void WindowsWindowManager::destroy_window(WindowID window_id)
{
    if (window_id == invalid_window_id || !m_windows.contains(window_id))
        return;

    HWND handle = m_windows.get(window_id).value().handle;
    m_windows.remove(window_id);

    // It is important to call this function after we removed the window ID from the
    // internal map to avoid receiving further messages from it.
    DestroyWindow(handle);
}

void WindowsWindowManager::show_window(WindowID window_id)
{
    if (window_id == invalid_window_id)
        return;

    ASSERT(m_windows.contains(window_id));
    auto& window_storage = m_windows[window_id];

    ShowWindow(window_storage.handle, SW_SHOW);
}

bool WindowsWindowManager::window_should_close(WindowID window_id)
{
    if (window_id == invalid_window_id || !m_windows.contains(window_id))
        return false;
    return m_windows.get(window_id).value().should_close;
}

Optional<Gfx::IntSize> WindowsWindowManager::get_window_size(WindowID window_id)
{
    if (window_id == invalid_window_id || !m_windows.contains(window_id))
        return {};

    auto const& window_storage = m_windows.get(window_id);
    HWND window_handle = window_storage.value().handle;

    RECT client_rect = {};
    if (!GetClientRect(window_handle, &client_rect))
        return {};

    LONG width = client_rect.right - client_rect.left;
    LONG height = client_rect.bottom - client_rect.top;
    if (width == 0 || height == 0)
        return {};

    return Gfx::IntSize(width, height);
}

void WindowsWindowManager::set_window_title(WindowID, String)
{
    ASSERT_NOT_REACHED;
}

void WindowsWindowManager::set_window_mode(WindowID, WindowMode)
{
    ASSERT_NOT_REACHED;
}

void WindowsWindowManager::set_window_size(WindowID, Gfx::IntSize)
{
    ASSERT_NOT_REACHED;
}

void WindowsWindowManager::set_on_window_requested_close_callback(WindowID window_id, OnWindowRequestedCloseCallback callback)
{
    if (window_id == invalid_window_id || !m_windows.contains(window_id))
        return;

    auto& window_storage = m_windows[window_id];
    window_storage.on_requested_close = move(callback);
}

void WindowsWindowManager::set_on_window_resized_callback(WindowID window_id, OnWindowResizedCallback callback)
{
    if (window_id == invalid_window_id || !m_windows.contains(window_id))
        return;

    auto& window_storage = m_windows.get(window_id).value();
    window_storage.on_resized = move(callback);
}

WindowID WindowsWindowManager::find_window_id_from_handle(HWND handle)
{
    WindowID result = invalid_window_id;
    m_windows.for_each([&](WindowID window_id, WindowStorage const& window_storage) {
        if (window_storage.handle == handle) {
            result = window_id;
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
        WindowID window_id = manager.find_window_id_from_handle(handle);
        auto& window_storage = manager.m_windows.get(window_id).value();
        window_storage.should_close = true;
        if (!window_storage.on_requested_close.is_empty())
            window_storage.on_requested_close(window_id);
        return 0;
    }

    case WM_SIZE: {
        WORD new_width = LOWORD(l_param);
        WORD new_height = HIWORD(l_param);

        WindowID window_id = manager.find_window_id_from_handle(handle);
        auto& window_storage = manager.m_windows.get(window_id).value();
        if (!window_storage.on_resized.is_empty())
            window_storage.on_resized(window_id, { new_width, new_height });
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
