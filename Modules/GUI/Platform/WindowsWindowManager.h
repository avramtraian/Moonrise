/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Types.h>
#if AND_PLATFORM_OS_WINDOWS

#    include <GUI/Platform/WindowManager.h>

#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <AND/HashMap.h>
#    include <AND/Utf16String.h>

#    include <Windows.h>

namespace GUI {

class WindowsWindowManager final : public WindowManager {
public:
    WindowsWindowManager() = default;
    virtual ~WindowsWindowManager() override = default;
    static WindowsWindowManager& the() { return reinterpret_cast<WindowsWindowManager&>(WindowManager::the()); }

    struct WindowStorage {
        HWND handle { nullptr };
        bool should_close { false };
        OnWindowRequestedCloseCallback on_requested_close;
        OnWindowResizedCallback on_resized;
    };

public:
    virtual void process_event_queue() override;

    virtual WindowID create_window() override;
    virtual void destroy_window(WindowID) override;
    virtual void show_window(WindowID) override;

    virtual bool window_should_close(WindowID) override;
    virtual Optional<Gfx::IntSize> get_window_size(WindowID) override;

    virtual void set_window_title(WindowID, String) override;
    virtual void set_window_mode(WindowID, WindowMode) override;
    virtual void set_window_size(WindowID, Gfx::IntSize) override;

    virtual void set_on_window_requested_close_callback(WindowID, OnWindowRequestedCloseCallback) override;
    virtual void set_on_window_resized_callback(WindowID, OnWindowResizedCallback) override;

private:
    virtual void initialize_impl() override;
    virtual void shutdown_impl() override;

    WindowID find_window_id_from_handle(HWND);
    static LRESULT CALLBACK win32_window_proc(HWND, UINT, WPARAM, LPARAM);
    void win32_register_class();

private:
    HashMap<WindowID, WindowStorage> m_windows;
    WindowID m_last_generated_window_id { invalid_window_id };
    Optional<Utf16String> m_window_class_name;
};

} // namespace GUI

#endif
