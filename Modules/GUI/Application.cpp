/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Application.h>

namespace GUI {

Application::Application(int, char**)
{
}

int Application::execute()
{
    return 0;
}

void Application::add_window(NonnullRefPtr<Window> const& window)
{
    m_windows.push_back(window);
}

void Application::add_panel(NonnullRefPtr<Panel> const& panel)
{
    m_panels.push_back(panel);
}

} // namespace GUI
