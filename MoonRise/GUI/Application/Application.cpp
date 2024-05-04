/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <MoonRise/GUI/Application/Application.h>

namespace MoonRise::GUI {

Application* g_application = nullptr;

ErrorOr<void> Application::create_window(const WindowInfo& window_info)
{
    OwnPtr<Window> window = Window::instantiate();
    if (!window.is_valid())
        return Error::OutOfMemory;
    m_window_stack.add(move(window));

    TRY(m_window_stack.last()->initialize(window_info));
    return {};
}

Window* Application::find_window_by_native_handle(void* native_handle)
{
    for (OwnPtr<Window>& window : m_window_stack) {
        if (window->native()->get_handle() == native_handle)
            return &window.get();
    }

    return {};
}

Window* Application::find_window_by_reference(WindowReference reference)
{
    return find_window_by_native_handle(reference.native_handle);
}

} // namespace MoonRise::GUI