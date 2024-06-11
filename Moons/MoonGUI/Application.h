/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Error.h>
#include <AT/RefPtr.h>
#include <MoonGUI/Forward.h>
#include <MoonGUI/GUI.h>

namespace GUI {

class Application {
    AT_MAKE_NONCOPYABLE(Application);
    AT_MAKE_NONMOVABLE(Application);

public:
    NODISCARD GUI_API static Application& get();

    GUI_API static ErrorOr<void> construct();
    GUI_API static void destruct();

public:
    GUI_API ErrorOr<RefPtr<Window>> construct_window(u32 window_width, u32 window_height);

private:
    Application() = default;
    ~Application() = default;
};

} // namespace GUI
