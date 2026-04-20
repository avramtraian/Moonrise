/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Application.h>
#include <GUI/Window.h>

int main(int argument_count, char** arguments)
{
    auto application = GUI::Application::construct(argument_count, arguments);
    auto window = GUI::Window::construct();
    application->add_window(window);

    window->show();
    return application->execute();
}
