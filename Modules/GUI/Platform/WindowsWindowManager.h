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
        HDC device_context { nullptr };
        bool should_close { false };
    };

public:
    virtual void process_event_queue() override;

    virtual NativeWindowHandle create_window() override;
    virtual void destroy_window(NativeWindowHandle) override;
    virtual void show_window(NativeWindowHandle) override;

    virtual bool window_should_close(NativeWindowHandle) override;
    virtual Optional<Gfx::IntSize> get_window_size(NativeWindowHandle) override;

    virtual void set_window_title(NativeWindowHandle, String) override;
    virtual void set_window_mode(NativeWindowHandle, WindowMode) override;
    virtual void set_window_size(NativeWindowHandle, Gfx::IntSize) override;

    virtual void present_back_buffer(NativeWindowHandle, NonnullRefPtr<Gfx::Bitmap> const&) override;

private:
    virtual void initialize_impl() override;
    virtual void shutdown_impl() override;

    NativeWindowHandle find_window_from_handle(HWND);
    static LRESULT CALLBACK win32_window_proc(HWND, UINT, WPARAM, LPARAM);
    void win32_register_class();

private:
    HashMap<NativeWindowHandle, WindowStorage> m_windows;
    NativeWindowHandle m_last_generated_window { invalid_native_window_handle };
    Optional<Utf16String> m_window_class_name;
};

} // namespace GUI

#endif
