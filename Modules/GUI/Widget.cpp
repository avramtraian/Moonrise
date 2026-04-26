/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Log.h>
#include <GUI/Events/Event.h>
#include <GUI/Events/LayoutEvent.h>
#include <GUI/Events/PaintEvent.h>
#include <GUI/Widget.h>

namespace GUI {

void Widget::notify(Event const& event)
{
#define ENUMERATE_EVENT_TYPE(Type, type) \
    if (event.is_##type##_event())       \
        return on_##type##_event(event.as_##type##_event());
    GUI_ENUMERATE_EVENT_TYPES(ENUMERATE_EVENT_TYPE)
#undef ENUMERATE_EVENT_TYPE
    on_unhandled_event(event);
}

void Widget::on_unhandled_event(Event const& event)
{
    outln("Widget of type '{}' received an unknown event of type '{}'!", class_name(), event.class_name());
}

void Widget::on_layout_event(LayoutEvent const& event)
{
    m_layout_region = event.region();
    m_needs_layout_update = false;
}

void Widget::on_paint_event(PaintEvent const& event)
{
    m_needs_paint_update = false;
}

void Widget::on_mouse_event(MouseEvent const& event)
{
    switch (event.type()) {
    case MouseEvent::Type::Moved:
        return on_mouse_moved_event(event);
    case MouseEvent::Type::ButtonPressed:
        return on_mouse_button_pressed_event(event);
    case MouseEvent::Type::ButtonReleased:
        return on_mouse_button_released_event(event);
    case MouseEvent::Type::WheelScrolled:
        return on_mouse_wheel_scrolled_event(event);
    default:
        ASSERT_NOT_REACHED;
    }
}

void Widget::on_mouse_moved_event(MouseEvent const&)
{
}

void Widget::on_mouse_button_pressed_event(MouseEvent const&)
{
}

void Widget::on_mouse_button_released_event(MouseEvent const&)
{
}

void Widget::on_mouse_wheel_scrolled_event(MouseEvent const&)
{
}

} // namespace GUI
