/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Defines.h>
#if AND_PLATFORM_OS_WINDOWS

#    include <GUI/Application.h>
#    include <GUI/Platform/WindowsEventLoop.h>

namespace GUI {

void WindowsEventLoop::execute()
{
    MSG message = {};
    while (!m_quited && GetMessageW(&message, nullptr, 0, 0) > 0) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
}

NativeTimerHandle WindowsEventLoop::create_timer()
{
    NativeTimerHandle timer_id = ++m_last_generated_timer_handle;
    auto& timer_storage = m_timers[timer_id];
    timer_storage.timeout = TimeDuration::from_seconds(0);
    return timer_id;
}

void WindowsEventLoop::destroy_timer(NativeTimerHandle timer_id)
{
    if (timer_id == invalid_native_timer_handle || !m_timers.contains(timer_id))
        return;
    m_timers.remove(timer_id);
}

void WindowsEventLoop::set_timer_timeout(NativeTimerHandle timer_id, TimeDuration timeout)
{
    if (timer_id == invalid_native_timer_handle || !m_timers.contains(timer_id))
        return;
    auto& timer_storage = m_timers[timer_id];
    timer_storage.timeout = timeout;

    post_timer_message(timer_id, timer_storage);
}

void WindowsEventLoop::win32_on_timer_timeout(HWND, UINT, UINT_PTR id_event, DWORD system_time_in_ms)
{
    auto& event_loop = reinterpret_cast<WindowsEventLoop&>(Application::the().event_loop());

    // Check that the event ID corresponds to a valid timer ID.
    NativeTimerHandle timer_handle = invalid_native_timer_handle;
    event_loop.m_timers.for_each([&](NativeTimerHandle handle, TimerStorage const& storage) {
        if (storage.id == id_event) {
            timer_handle = handle;
            return IterationDecision::Break;
        }
        return IterationDecision::Continue;
    });

    if (timer_handle == invalid_native_timer_handle)
        return;

    // Dispatch the event callback.
    if (event_loop.on_timer_timeout.is_valid())
        event_loop.on_timer_timeout(timer_handle);
}

void WindowsEventLoop::post_timer_message(NativeTimerHandle timer_id, TimerStorage& timer_storage)
{
    DWORD timeout_in_ms = static_cast<DWORD>(timer_storage.timeout.milliseconds());
    timer_storage.id = SetTimer(nullptr, timer_id, timeout_in_ms, win32_on_timer_timeout);
}

} // namespace GUI

#endif
