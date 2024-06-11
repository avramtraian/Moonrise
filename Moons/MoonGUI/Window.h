/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Badge.h>
#include <AT/Error.h>
#include <AT/RefPtr.h>
#include <MoonGUI/Forward.h>
#include <MoonGUI/Native/Window.h>

namespace GUI {

class Window : public RefCounted<Window> {
public:
    Window(Badge<Application>) {}
    ~Window() = default;

    ErrorOr<void> construct(u32 window_width, u32 window_height, Badge<Application>);

public:
    NODISCARD ALWAYS_INLINE u32 get_client_width() const { return m_native_window.get_client_area_width(); }
    NODISCARD ALWAYS_INLINE u32 get_client_height() const { return m_native_window.get_client_area_height(); }

private:
    Native::Window m_native_window;
};

} // namespace GUI
