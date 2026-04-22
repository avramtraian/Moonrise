/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Application.h>
#include <GUI/Timer.h>

namespace GUI {

Timer::Timer()
    : m_native_handle(invalid_native_timer_handle)
{
}

Timer::~Timer()
{
    if (m_native_handle != invalid_native_timer_handle)
        Application::the().unregister_timer(m_native_handle);
}

void Timer::initialize(TimeDuration timeout, Function<void()> callback)
{
    ASSERT(m_native_handle == invalid_native_timer_handle);

    auto& application = Application::the();
    auto& event_loop = application.event_loop();

    m_native_handle = event_loop.create_timer();
    application.register_timer(m_native_handle, move(callback));
    event_loop.set_timer_timeout(m_native_handle, timeout);
}

} // namespace GUI
