/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Containers/BorderBox.h>
#include <GUI/Events/LayoutEvent.h>
#include <GUI/Events/PaintEvent.h>
#include <Gfx/Painter.h>

namespace GUI {

void BorderBox::set_widget(NonnullRefPtr<Widget> const& widget)
{
    m_widget = widget;
    schedule_layout_event();
    schedule_paint_event();
}

void BorderBox::set_background(Gfx::Color color)
{
    m_background_color = color;
    schedule_paint_event();
}

void BorderBox::set_color(Gfx::Color color)
{
    m_border_color = color;
    schedule_paint_event();
}

void BorderBox::set_border(Gfx::IntBorder border)
{
    m_border = border;
    schedule_layout_event();
    schedule_paint_event();
}

void BorderBox::on_layout_event(LayoutEvent const& event)
{
    Base::on_layout_event(event);
    if (!m_widget.is_valid())
        return;

    LayoutEvent layout_event { event.region().without_border(m_border) };
    m_widget->on_layout_event(layout_event);
}

void BorderBox::on_paint_event(PaintEvent const& event)
{
    Base::on_paint_event(event);
    if (!m_widget.is_valid())
        return;

    Gfx::Painter painter { event.paint_buffer(), m_layout_region };
    paint_border(painter);
    paint_background(painter);
    m_widget->on_paint_event(event);
}

Gfx::IntSize BorderBox::calculate_preferred_size() const
{
    auto widget_preferred_size = Gfx::IntSize::zero();
    if (m_widget.is_valid())
        widget_preferred_size = m_widget->calculate_preferred_size();
    return { widget_preferred_size.width() + m_border.horizontal(), widget_preferred_size.height() + m_border.vertical() };
}

void BorderBox::paint_border(Gfx::Painter& painter)
{
    painter.outline_rect(m_layout_region, m_border_color, m_border);
}

void BorderBox::paint_background(Gfx::Painter& painter)
{
    auto background_region = m_layout_region.without_border(m_border);
    painter.fill_rect(background_region, m_background_color);
}

} // namespace GUI
