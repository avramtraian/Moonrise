/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <MoonRise/GUI/Native/NativeWindow.h>

#ifndef NOMINMAX
    #define NOMINMAX
#endif // NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace MoonRise::GUI {

class WindowsNativeWindow : public NativeWindow {
public:
    static LRESULT window_procedure(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param);

public:
    WindowsNativeWindow() = default;
    virtual ~WindowsNativeWindow() = default;

    virtual ErrorOr<void> initialize(const WindowInfo& window_info) override;

    virtual void* get_handle() const override { return reinterpret_cast<void*>(m_native_handle); }

    virtual u32 get_client_width() const override { return m_client_area_width; }
    virtual u32 get_client_height() const override { return m_client_area_height; }
    virtual i32 get_client_position_x() const override { return m_client_area_position_x; }
    virtual i32 get_client_position_y() const override { return m_client_area_position_y; }

private:
    HWND m_native_handle;
    bool m_should_close;

    u32 m_client_area_width;
    u32 m_client_area_height;
    i32 m_client_area_position_x;
    i32 m_client_area_position_y;
    WindowStateMode m_state_mode;
};

} // namespace MoonRise::GUI