/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Function.h>
#include <AND/RefPtr.h>
#include <AND/String.h>
#include <GUI/Events/MouseEvent.h>
#include <GUI/Platform/NativeHandles.h>
#include <Gfx/Forward.h>
#include <Gfx/Size.h>

namespace GUI {

enum class WindowMode {
    Windowed,
    Fullscreen,
    Minimized,
    Maximized,
};

class WindowManager {
public:
    static void initialize();
    static void shutdown();
    static WindowManager& the();

public:
    virtual void process_event_queue() = 0;

    virtual NativeWindowHandle create_window() = 0;
    virtual void destroy_window(NativeWindowHandle) = 0;
    virtual void show_window(NativeWindowHandle) = 0;

    virtual bool window_should_close(NativeWindowHandle) = 0;
    virtual Optional<Gfx::IntSize> get_window_size(NativeWindowHandle) = 0;

    virtual void set_window_title(NativeWindowHandle, String) = 0;
    virtual void set_window_mode(NativeWindowHandle, WindowMode) = 0;
    virtual void set_window_size(NativeWindowHandle, Gfx::IntSize) = 0;

    // Returns the coordinates of the given point relative to the window's client region, which is
    // always (even though the native layer might disagree) the bottom-left corner.
    virtual Optional<Gfx::IntPoint> calculate_relative_position(NativeWindowHandle, Gfx::IntPoint) = 0;

    // FIXME: This function is really specific to the Software render drivee and the window manager
    //        is probably not the right place for it. However, since GPU rendering will be implemented
    //        probably very far in the future, it is really convenient to have it here... for now.
    virtual void present_back_buffer(NativeWindowHandle, NonnullRefPtr<Gfx::Bitmap> const&) = 0;

public:
    Function<void(NativeWindowHandle)> on_window_requested_close;
    Function<void(NativeWindowHandle, Gfx::IntSize)> on_window_resized;

    Function<void(Gfx::IntPoint)> on_mouse_moved;

protected:
    WindowManager() = default;
    virtual ~WindowManager() = default;

    virtual void initialize_impl() = 0;
    virtual void shutdown_impl() = 0;
};

} // namespace GUI
