/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Error.h>
#include <AT/OwnPtr.h>
#include <AT/String.h>
#include <MoonGUI/GUI.h>
#include <MoonGUI/Native/Windows/WindowsHeaders.h>

namespace GUI::Native {

class WindowsWindow {
    AT_MAKE_NONCOPYABLE(WindowsWindow);
    AT_MAKE_NONMOVABLE(WindowsWindow);

public:
    struct Info {
        u32 client_width { 0 };
        u32 client_height { 0 };
        i32 client_position_x { 0 };
        i32 client_position_y { 0 };
        String title { "Unnamed window"sv };
        bool start_maximized { true };
    };

public:
    static OwnPtr<WindowsWindow> instantiate();

    GUI_API ErrorOr<void> initialize(const Info& info);
    GUI_API ~WindowsWindow();

    static LRESULT window_procedure(HWND, UINT, WPARAM, LPARAM);

public:
    NODISCARD ALWAYS_INLINE void* get_native_handle() const { return m_native_handle; }

    NODISCARD ALWAYS_INLINE u32 get_client_area_width() const { return m_client_area_width; }
    NODISCARD ALWAYS_INLINE u32 get_client_area_height() const { return m_client_area_height; }
    NODISCARD ALWAYS_INLINE i32 get_client_area_position_x() const { return m_client_area_position_x; }
    NODISCARD ALWAYS_INLINE i32 get_client_area_position_y() const { return m_client_area_position_y; }

private:
    WindowsWindow() = default;

    void close();

private:
    void* m_native_handle { nullptr };
    bool m_should_close { false };

    u32 m_client_area_width { 0 };
    u32 m_client_area_height { 0 };
    i32 m_client_area_position_x { 0 };
    i32 m_client_area_position_y { 0 };
};

} // namespace GUI::Native
