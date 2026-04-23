/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/HashMap.h>
#include <AND/Noncopyable.h>
#include <AND/OwnPtr.h>
#include <AND/RefPtr.h>
#include <AND/Vector.h>
#include <GUI/Panel.h>
#include <GUI/Platform/EventLoop.h>
#include <GUI/Window.h>

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

public:
    void register_timer(NativeTimerHandle, Function<void()>);
    void unregister_timer(NativeTimerHandle);

    void add_window(NonnullRefPtr<Window> const&);
    void add_panel(NonnullRefPtr<Panel> const&);

private:
    Application() = default;
    void initialize(int argument_count, char** arguments);
    void destroy();

    void on_timer_timeout(NativeTimerHandle);

    void on_window_requested_close(NativeWindowHandle);
    void on_window_resized(NativeWindowHandle, Gfx::IntSize);
    void on_mouse_moved(Gfx::IntPoint);

private:
    OwnPtr<EventLoop> m_event_loop;
    HashMap<NativeTimerHandle, Function<void()>> m_timer_dispatch_map;
    Vector<NonnullRefPtr<Window>> m_windows;
    Vector<NonnullRefPtr<Panel>> m_panels;
};

} // namespace GUI
