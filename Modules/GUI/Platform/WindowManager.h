/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Function.h>
#include <AND/String.h>
#include <Gfx/Size.h>

namespace GUI {

using WindowID = u32;
static constexpr WindowID invalid_window_id = 0;

enum class WindowMode {
    Windowed,
    Fullscreen,
    Minimized,
    Maximized,
};

using OnWindowRequestedCloseCallback = Function<void(WindowID)>;
using OnWindowResizedCallback = Function<void(WindowID, Gfx::IntSize)>;

class WindowManager {
public:
    static void initialize();
    static void shutdown();
    static WindowManager& the();

public:
    virtual void process_event_queue() = 0;

    virtual WindowID create_window() = 0;
    virtual void destroy_window(WindowID) = 0;
    virtual void show_window(WindowID) = 0;

    virtual bool window_should_close(WindowID) = 0;
    virtual Optional<Gfx::IntSize> get_window_size(WindowID) = 0;

    virtual void set_window_title(WindowID, String) = 0;
    virtual void set_window_mode(WindowID, WindowMode) = 0;
    virtual void set_window_size(WindowID, Gfx::IntSize) = 0;

    virtual void set_on_window_requested_close_callback(WindowID, OnWindowRequestedCloseCallback) = 0;
    virtual void set_on_window_resized_callback(WindowID, OnWindowResizedCallback) = 0;

protected:
    WindowManager() = default;
    virtual ~WindowManager() = default;

    virtual void initialize_impl() = 0;
    virtual void shutdown_impl() = 0;
};

} // namespace GUI
