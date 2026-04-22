/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Function.h>
#include <AND/OwnPtr.h>
#include <AND/Platform.h>
#include <GUI/Platform/NativeHandles.h>
#include <Gfx/Size.h>

namespace GUI {

class EventLoop {
    AND_MAKE_NONCOPYABLE(EventLoop);
    AND_MAKE_NONMOVABLE(EventLoop);

public:
    static NonnullOwnPtr<EventLoop> construct();
    virtual ~EventLoop() = default;

    virtual void execute() = 0;
    virtual void quit() = 0;

    virtual NativeTimerHandle create_timer() = 0;
    virtual void destroy_timer(NativeTimerHandle) = 0;
    virtual void set_timer_timeout(NativeTimerHandle, TimeDuration) = 0;

public:
    Function<void(NativeTimerHandle)> on_timer_timeout;

    Function<void(NativeWindowHandle, Gfx::IntSize)> on_window_resized;
    Function<void(NativeWindowHandle)> on_window_requested_close;

protected:
    EventLoop() = default;
};

} // namespace GUI
