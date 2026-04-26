/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/HashMap.h>
#include <AND/OwnPtr.h>
#include <AND/RefPtr.h>
#include <AND/Vector.h>
#include <GUI/Cursor.h>
#include <GUI/Forward.h>
#include <GUI/Platform/EventLoop.h>
#include <Gfx/Point.h>

namespace GUI {

class Application {
    AND_MAKE_NONCOPYABLE(Application);
    AND_MAKE_NONMOVABLE(Application);

public:
    static NonnullOwnPtr<Application> construct(int argument_count, char** arguments);
    ~Application();

    static Application& the();
    int execute();

    EventLoop& event_loop() { return *m_event_loop; }
    Cursor& cursor() { return *m_cursor; }

public:
    void register_timer(NativeTimerHandle, Function<void()>);
    void unregister_timer(NativeTimerHandle);

    void add_window(NonnullRefPtr<Window> const&);
    void add_panel(NonnullRefPtr<Panel> const&);

    template<typename WindowType, typename... Args>
    requires(is_derived_from<WindowType, Window>)
    NonnullRefPtr<WindowType> add_window(Args&&... args)
    {
        auto window = WindowType::construct(forward<Args>(args)...);
        add_window(window);
        return window;
    }

private:
    Application() = default;
    void initialize(int argument_count, char** arguments);
    void destroy();

    void on_timer_timeout(NativeTimerHandle);

    void on_window_requested_close(NativeWindowHandle);
    void on_window_resized(NativeWindowHandle, Gfx::IntSize);

    void on_mouse_moved(Gfx::IntPoint);
    void on_mouse_button_pressed_event(NativeWindowHandle, MouseButton);
    void on_mouse_button_released_event(NativeWindowHandle, MouseButton);
    void on_mouse_wheel_scrolled_event(NativeWindowHandle, float delta_x, float delta_y);

private:
    OwnPtr<EventLoop> m_event_loop;
    HashMap<NativeTimerHandle, Function<void()>> m_timer_dispatch_map;
    Vector<NonnullRefPtr<Window>> m_windows;
    Vector<NonnullRefPtr<Panel>> m_panels;
    RefPtr<Cursor> m_cursor;
};

} // namespace GUI
