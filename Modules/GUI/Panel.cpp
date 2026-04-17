/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Panel.h>

namespace GUI {

void Panel::set_main_widget(NonnullRefPtr<Widget> const& widget)
{
    m_main_widget = widget;
}

} // namespace GUI
