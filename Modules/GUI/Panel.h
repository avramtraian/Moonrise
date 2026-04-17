/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <GUI/Widget.h>

namespace GUI {

class Panel : public Widget {
    GUI_OBJECT(Panel, Widget);

public:
    void set_main_widget(NonnullRefPtr<Widget> const& widget);

private:
    RefPtr<Widget> m_main_widget;
};

} // namespace GUI
