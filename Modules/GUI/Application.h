/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Noncopyable.h>
#include <AND/RefPtr.h>
#include <AND/Vector.h>
#include <GUI/Panel.h>
#include <GUI/Window.h>

namespace GUI {

class Application {
    AND_MAKE_NONCOPYABLE(Application);
    AND_MAKE_NONMOVABLE(Application);

public:
    Application(int argument_count, char** arguments);
    int execute();

public:
    void add_window(NonnullRefPtr<Window> const&);
    void add_panel(NonnullRefPtr<Panel> const&);

    template<typename T, typename... Args>
    requires(is_derived_from<T, Window>)
    NonnullRefPtr<T> construct_window(Args&&... args)
    {
        auto window = make_ref<T>(forward<Args>(args)...);
        add_window(window);
        return window;
    }

    template<typename T, typename... Args>
    requires(is_derived_from<T, Panel>)
    NonnullRefPtr<T> construct_panel(Args&&... args)
    {
        auto panel = make_ref<T>(forward<Args>(args)...);
        add_panel(panel);
        return panel;
    }

private:
    Vector<NonnullRefPtr<Window>> m_windows;
    Vector<NonnullRefPtr<Panel>> m_panels;
};

} // namespace GUI
