/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Error.h"
#include "AT/Vector.h"
#include "MoonRise/GUI/Application/Window.h"

namespace MoonRise::GUI {

struct WindowReference {
    void* native_handle;
};

class Application {
public:
    ErrorOr<void> create_window(const WindowInfo& window_info);
    Window* find_window_by_native_handle(void* native_handle);
    Window* find_window_by_reference(WindowReference reference);

private:
    Vector<OwnPtr<Window>> m_window_stack;
};

extern Application* g_application;

} // namespace MoonRise::GUI