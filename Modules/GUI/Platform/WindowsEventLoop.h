/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Defines.h>
#if AND_PLATFORM_OS_WINDOWS

#    include <AND/HashMap.h>
#    include <AND/HashSet.h>
#    include <GUI/Platform/EventLoop.h>

#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <Windows.h>

namespace GUI {

class WindowsEventLoop final : public EventLoop {
public:
    struct TimerStorage {
        UINT_PTR id;
        TimeDuration timeout;
    };

public:
    WindowsEventLoop() = default;
    virtual ~WindowsEventLoop() override = default;

    virtual void execute() override;
    virtual void quit() override { m_quited = true; }

    virtual NativeTimerHandle create_timer() override;
    virtual void destroy_timer(NativeTimerHandle) override;
    virtual void set_timer_timeout(NativeTimerHandle, TimeDuration) override;

private:
    static void win32_on_timer_timeout(HWND window_handle, UINT message_id, UINT_PTR id_event, DWORD system_time_in_ms);
    void post_timer_message(NativeTimerHandle, TimerStorage&);

private:
    bool m_quited { false };
    HashMap<NativeTimerHandle, TimerStorage> m_timers;
    NativeTimerHandle m_last_generated_timer_handle { invalid_native_timer_handle };
    HashSet<NativeWindowHandle> m_registered_windows;
};

} // namespace GUI

#endif
