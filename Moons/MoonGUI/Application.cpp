/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <MoonGUI/Application.h>
#include <MoonGUI/Window.h>

namespace GUI {

static Application* s_application_instance;

Application& Application::get()
{
    AT_ASSERT(s_application_instance);
    return *s_application_instance;
}

ErrorOr<void> Application::construct()
{
    AT_ASSERT(!s_application_instance);
    s_application_instance = new Application;
    return {};
}

void Application::destruct()
{
    AT_ASSERT(s_application_instance);
    delete s_application_instance;
    s_application_instance = nullptr;
}

ErrorOr<RefPtr<Window>> Application::construct_window(u32 window_width, u32 window_height)
{
    RefPtr<Window> window = adopt_ref(new Window({}));
    TRY(window->construct(window_width, window_height, {}));
    return window;
}

} // namespace GUI
