/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "MoonRise/GUI/Application/Window.h"
#include "AT/OwnPtr.h"

namespace MoonRise::GUI {

OwnPtr<Window> Window::instantiate()
{
    return make_own<Window>();
}

ErrorOr<void> Window::initialize(const WindowInfo& window_info)
{
    m_native_window = NativeWindow::instantiate();
    if (!m_native_window.is_valid())
        return Error::OutOfMemory;

    TRY(m_native_window->initialize(window_info));
    return {};
}

} // namespace MoonRise::GUI