/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Noncopyable.h>
#include <AND/OwnPtr.h>
#include <AND/RefPtr.h>
#include <AND/Vector.h>
#include <GUI/Panel.h>
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

public:
    void add_window(NonnullRefPtr<Window> const&);
    void add_panel(NonnullRefPtr<Panel> const&);

private:
    Application() = default;
    void initialize(int argument_count, char** arguments);
    void destroy();

private:
    Vector<NonnullRefPtr<Window>> m_windows;
    Vector<NonnullRefPtr<Panel>> m_panels;
};

} // namespace GUI
