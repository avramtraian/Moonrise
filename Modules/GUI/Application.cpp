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
}

void Application::destroy()
{
    Gfx::RenderDriver::shutdown();
    GUI::WindowManager::shutdown();
}

int Application::execute()
{
    while (!m_windows.is_empty()) {
        WindowManager::the().process_event_queue();

        Vector<usize, 8> window_indices_to_remove;
        for (usize index = 0; index < m_windows.count(); ++index) {
            if (m_windows[index]->should_close())
                window_indices_to_remove.push_back(index);
        }

        for (usize index : window_indices_to_remove)
            m_windows.remove_at(index);
    }

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
