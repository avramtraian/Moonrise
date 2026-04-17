/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefPtr.h>
#include <GUI/Object.h>
#include <GUI/Widget.h>

namespace GUI {

class Window : public Object {
    GUI_OBJECT(Window, Object);

public:
    void set_main_widget(NonnullRefPtr<Widget> const&);
    void show();

private:
    RefPtr<Widget> m_main_widget;
};

} // namespace GUI
