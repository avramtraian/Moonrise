/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Error.h>
#include <MoonGUI/GUI.h>

namespace GUI {

class Application {
    AT_MAKE_NONCOPYABLE(Application);
    AT_MAKE_NONMOVABLE(Application);

public:
    struct Info {};

public:
    Application() = default;
    virtual ~Application() = default;

    GUI_API static ErrorOr<void> create(const Info& info);
    GUI_API static void destroy();

    NODISCARD GUI_API static Application& get();
};

} // namespace GUI
