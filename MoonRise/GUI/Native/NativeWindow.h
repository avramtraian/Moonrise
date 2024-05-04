/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Error.h"
#include "AT/OwnPtr.h"
#include "AT/String.h"

namespace MoonRise::GUI {

struct WindowInfo {
    u32 width = 800;
    u32 height = 600;
    i32 position_x = 500;
    i32 position_y = 500;
    String title = "Untitled MoonRise Window"sv;

    bool start_in_fullscreen = false;
    bool start_maximized = false;
    bool start_minimized = false;
};

enum WindowStateMode {
    Normal,
    Maximized,
    Minimized,
    Fullscreen,
};

class NativeWindow {
    AT_MAKE_NONCOPYABLE(NativeWindow);
    AT_MAKE_NONMOVABLE(NativeWindow);

public:
    virtual ~NativeWindow() = default;

    static OwnPtr<NativeWindow> instantiate();
    virtual ErrorOr<void> initialize(const WindowInfo& window_info) = 0;

    virtual void* get_handle() const = 0;

    virtual u32 get_client_width() const = 0;
    virtual u32 get_client_height() const = 0;
    virtual i32 get_client_position_x() const = 0;
    virtual i32 get_client_position_y() const = 0;

protected:
    NativeWindow() = default;
};

} // namespace MoonRise::GUI