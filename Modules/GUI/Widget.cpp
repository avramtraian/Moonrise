/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Events/LayoutEvent.h>
#include <GUI/Events/PaintEvent.h>
#include <GUI/Widget.h>

namespace GUI {

void Widget::on_layout_event(LayoutEvent const& event)
{
    m_layout_region = event.region();
}

void Widget::on_paint_event(PaintEvent const& event)
{
}

void Widget::on_mouse_moved_event(MouseEvent const&)
{
}

} // namespace GUI
