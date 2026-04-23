/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Application.h>
#include <GUI/Platform/WindowManager.h>
#include <Gfx/RenderDriver.h>

namespace GUI {

static Application* s_the_application;

NonnullOwnPtr<Application> Application::construct(int argument_count, char** arguments)
{
    ASSERT(!s_the_application);
    auto application = adopt_nonnull_own(*new Application());
    s_the_application = application.get();

    application->initialize(argument_count, arguments);
    return application;
}

Application& Application::the()
{
    ASSERT(s_the_application);
    return *s_the_application;
}

Application::~Application()
{
    ASSERT(s_the_application);
    ASSERT(s_the_application == this);
    s_the_application = nullptr;
}

void Application::initialize(int, char**)
{
    GUI::WindowManager::initialize();
    Gfx::RenderDriver::initialize(Gfx::RenderDriverType::Software);
    m_event_loop = EventLoop::construct();
}

void Application::destroy()
{
    m_event_loop.release();
    Gfx::RenderDriver::shutdown();
    GUI::WindowManager::shutdown();
}

int Application::execute()
{
    //
    // Redirect the events received by the window manager to the application event loop.
    //
    WindowManager::the().on_window_requested_close = [this](NativeWindowHandle window) {
        on_window_requested_close(window);
    };
    WindowManager::the().on_window_resized = [this](NativeWindowHandle window, Gfx::IntSize new_size) {
        on_window_resized(window, new_size);
    };
    WindowManager::the().on_mouse_moved = [this](Gfx::IntPoint absolute_position) {
        on_mouse_moved(absolute_position);
    };

    //
    // Redirect the events received by the event loop to the application.
    //
    m_event_loop->on_timer_timeout = [this](NativeTimerHandle timer_id) {
        on_timer_timeout(timer_id);
    };
    m_event_loop->on_window_requested_close = [this](NativeWindowHandle window) {
        on_window_requested_close(window);
    };
    m_event_loop->on_window_resized = [this](NativeWindowHandle window, Gfx::IntSize new_size) {
        on_window_resized(window, new_size);
    };

    m_event_loop->execute();
    m_event_loop.release();
    return 0;
}

void Application::register_timer(NativeTimerHandle timer, Function<void()> callback)
{
    if (timer == invalid_native_timer_handle)
        return;
    ASSERT(!m_timer_dispatch_map.contains(timer));
    m_timer_dispatch_map.add(timer, move(callback));
}

void Application::unregister_timer(NativeTimerHandle timer)
{
    if (timer == invalid_native_timer_handle)
        return;
    ASSERT(m_timer_dispatch_map.contains(timer));
    m_timer_dispatch_map.remove(timer);
}

void Application::add_window(NonnullRefPtr<Window> const& window)
{
    m_windows.push_back(window);
}

void Application::add_panel(NonnullRefPtr<Panel> const& panel)
{
    m_panels.push_back(panel);
}

void Application::on_timer_timeout(NativeTimerHandle timer)
{
    if (timer == invalid_native_timer_handle || !m_timer_dispatch_map.contains(timer))
        return;

    auto& callback = m_timer_dispatch_map.get(timer).value();
    if (callback.is_valid())
        callback();
}

void Application::on_window_requested_close(NativeWindowHandle window_handle)
{
    if (window_handle == invalid_native_window_handle)
        return;

    m_windows.remove_all_matching([&](auto const& window) {
        if (window->native_handle() == window_handle)
            return MatchResult::Yes;
        return MatchResult::No;
    });

    if (m_windows.is_empty())
        m_event_loop->quit();
}

void Application::on_window_resized(NativeWindowHandle window_handle, Gfx::IntSize new_size)
{
    m_windows.for_each([&](auto& window) {
        if (window->native_handle() == window_handle) {
            window->on_resize_event(new_size);
            return IterationDecision::Break;
        }
        return IterationDecision::Continue;
    });
}

void Application::on_mouse_moved(Gfx::IntPoint absolute_position)
{
    m_windows.for_each([&](auto const& window) {
        auto relative_position = WindowManager::the().calculate_relative_position(window->native_handle(), absolute_position);
        if (!relative_position.has_value())
            return IterationDecision::Continue;

        // FIXME: We should consider the Z-order when deciding which window receives the mouse-moved event.
        //        For now, we send the events to all windows which happen to contain the mouse cursor.

        window->on_mouse_moved_event(relative_position.value());
        return IterationDecision::Continue;
    });
}

} // namespace GUI
