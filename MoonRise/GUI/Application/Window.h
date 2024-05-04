/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "MoonRise/GUI/Native/NativeWindow.h"

namespace MoonRise::GUI {

class Window {
    AT_MAKE_NONCOPYABLE(Window);
    AT_MAKE_NONMOVABLE(Window);

public:
    Window() = default;
    ~Window() = default;

    static OwnPtr<Window> instantiate();
    ErrorOr<void> initialize(const WindowInfo& window_info);

    ALWAYS_INLINE OwnPtr<NativeWindow>& native() { return m_native_window; }
    ALWAYS_INLINE const OwnPtr<NativeWindow>& native() const { return m_native_window; }

private:
    OwnPtr<NativeWindow> m_native_window;
};

} // namespace MoonRise::GUI