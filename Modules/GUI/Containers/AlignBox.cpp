/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Containers/AlignBox.h>
#include <GUI/Events/LayoutEvent.h>
#include <GUI/Events/PaintEvent.h>

namespace GUI {

Alignment operator|(VAlignment vertical, HAlignment horizontal)
{
    switch (vertical) {
    case VAlignment::Bottom:
        switch (horizontal) {
        case HAlignment::Left:
            return Alignment::BottomLeft;
        case HAlignment::Center:
            return Alignment::BottomCenter;
        case HAlignment::Right:
            return Alignment::BottomRight;
        }
        break;
    case VAlignment::Center:
        switch (horizontal) {
        case HAlignment::Left:
            return Alignment::CenterLeft;
        case HAlignment::Center:
            return Alignment::Center;
        case HAlignment::Right:
            return Alignment::CenterRight;
        }
        break;
    case VAlignment::Top:
        switch (horizontal) {
        case HAlignment::Left:
            return Alignment::TopLeft;
        case HAlignment::Center:
            return Alignment::TopCenter;
        case HAlignment::Right:
            return Alignment::TopRight;
        }
        break;
    }

    ASSERT_NOT_REACHED;
}

Alignment operator|(HAlignment horizontal, VAlignment vertical)
{
    return vertical | horizontal;
}

void AlignBox::set_widget(NonnullRefPtr<Widget> const& widget)
{
    m_widget = widget;
    schedule_layout_event();
    schedule_paint_event();
}

void AlignBox::set_alignment(Alignment alignment)
{
    switch (alignment) {
    case Alignment::BottomLeft:
        set_alignment({ 0.0F, 0.0F });
        set_child_anchor({ 0.0F, 0.0F });
        break;
    case Alignment::BottomCenter:
        set_alignment({ 0.5F, 0.0F });
        set_child_anchor({ 0.5F, 0.0F });
        break;
    case Alignment::BottomRight:
        set_alignment({ 1.0F, 0.0F });
        set_child_anchor({ 1.0F, 0.0F });
        break;
    case Alignment::CenterLeft:
        set_alignment({ 0.0F, 0.5F });
        set_child_anchor({ 0.0F, 0.5F });
        break;
    case Alignment::Center:
        set_alignment({ 0.5F, 0.5F });
        set_child_anchor({ 0.5F, 0.5F });
        break;
    case Alignment::CenterRight:
        set_alignment({ 1.0F, 0.5F });
        set_child_anchor({ 1.0F, 0.5F });
        break;
    case Alignment::TopLeft:
        set_alignment({ 0.0F, 1.0F });
        set_child_anchor({ 0.0F, 1.0F });
        break;
    case Alignment::TopCenter:
        set_alignment({ 0.5F, 1.0F });
        set_child_anchor({ 0.5F, 1.0F });
        break;
    case Alignment::TopRight:
        set_alignment({ 1.0F, 1.0F });
        set_child_anchor({ 1.0F, 1.0F });
        break;
    default:
        ASSERT_NOT_REACHED;
    }
}

void AlignBox::set_alignment(Gfx::FloatPoint alignment)
{
    auto old_alignment = m_alignment;
    m_alignment.x = clamp(alignment.x, 0.0F, 1.0F);
    m_alignment.y = clamp(alignment.y, 0.0F, 1.0F);
    if (m_alignment != old_alignment)
        schedule_layout_event();
}

void AlignBox::set_child_anchor(Gfx::FloatPoint child_anchor)
{
    auto old_child_anchor = m_child_anchor;
    m_child_anchor.x = clamp(child_anchor.x, 0.0F, 1.0F);
    m_child_anchor.y = clamp(child_anchor.y, 0.0F, 1.0F);
    if (m_child_anchor != old_child_anchor)
        schedule_layout_event();
}

void AlignBox::set_offset(s32 offset_x, s32 offset_y)
{
    auto old_offset_x = m_offset_x;
    auto old_offset_y = m_offset_y;
    m_offset_x = offset_x;
    m_offset_y = offset_y;
    if (m_offset_x != old_offset_x || m_offset_y != old_offset_y)
        schedule_layout_event();
}

void AlignBox::set_width_policy(SizePolicy policy)
{
    if (m_width_policy == policy)
        return;

    m_width_policy = policy;
    if (m_width_policy == SizePolicy::ExplicitLength)
        m_fixed_width = Length::from_percentage(0.5F);
    else
        m_fixed_width.release();

    schedule_layout_event();
}

void AlignBox::set_height_policy(SizePolicy policy)
{
    if (m_height_policy == policy)
        return;

    m_height_policy = policy;
    if (m_height_policy == SizePolicy::ExplicitLength)
        m_fixed_height = Length::from_percentage(0.5F);
    else
        m_fixed_height.release();

    schedule_layout_event();
}

void AlignBox::set_preferred_width(Length width)
{
    if (m_width_policy == SizePolicy::ExplicitLength) {
        ASSERT(m_fixed_width.has_value());
        if (m_fixed_width.value() == width)
            return;
    }

    m_width_policy = SizePolicy::ExplicitLength;
    m_fixed_width = width;
    schedule_layout_event();
}

void AlignBox::set_preferred_height(Length height)
{
    if (m_height_policy == SizePolicy::ExplicitLength) {
        ASSERT(m_fixed_height.has_value());
        if (m_fixed_height.value() == height)
            return;
    }

    m_height_policy = SizePolicy::ExplicitLength;
    m_fixed_height = height;
    schedule_layout_event();
}

void AlignBox::notify(Event const& event)
{
    if (!m_widget.is_valid())
        return Base::notify(event);

    // If the widget is valid and the event is not a layout, paint, or mouse event, we should forward the event handling to the widget.
    if (!event.is_layout_event() && !event.is_paint_event() && !event.is_mouse_event())
        return m_widget->notify(event);

    if (event.is_mouse_event()) {
        // Forward the event to the child widget if the mouse position is within the widget's layout region.
        auto const& mouse_event = event.as_mouse_event();
        if (m_widget->layout_region().contains(mouse_event.position()))
            return m_widget->notify(mouse_event);
    }

    Base::notify(event);
}

void AlignBox::on_layout_event(LayoutEvent const& event)
{
    Base::on_layout_event(event);
    if (!m_widget.is_valid())
        return;

    // Calculate the widget size.
    //--------------------------------------------------------------------------
    auto widget_width = calculate_preferred_widget_width();
    auto widget_height = calculate_preferred_widget_height();

    widget_width = clamp<u32>(widget_width, 0, m_layout_region.width());
    widget_height = clamp<u32>(widget_height, 0, m_layout_region.height());

    // Calculate the widget bottom-left corner offset.
    //--------------------------------------------------------------------------
    Gfx::IntPoint alignment_offset;
    alignment_offset.x = static_cast<s32>(m_layout_region.width() * m_alignment.x);
    alignment_offset.y = static_cast<s32>(m_layout_region.height() * m_alignment.y);

    Gfx::IntPoint child_anchor_offset;
    child_anchor_offset.x = static_cast<s32>(widget_width * m_child_anchor.x);
    child_anchor_offset.y = static_cast<s32>(widget_height * m_child_anchor.y);

    Gfx::IntPoint widget_offset;
    widget_offset.x = alignment_offset.x - child_anchor_offset.x + m_offset_x;
    widget_offset.y = alignment_offset.y - child_anchor_offset.y + m_offset_y;

    // Re-layout the widget.
    //--------------------------------------------------------------------------
    Gfx::IntRect widget_region;
    widget_region.set_offset(m_layout_region.offset_x() + widget_offset.x, m_layout_region.offset_y() + widget_offset.y);
    widget_region.set_size(widget_width, widget_height);
    widget_region = Gfx::IntRect::intersect(widget_region, m_layout_region);

    LayoutEvent layout_event { widget_region };
    m_widget->notify(layout_event);
}

void AlignBox::on_paint_event(PaintEvent const& event)
{
    Base::on_paint_event(event);
    if (!m_widget.is_valid())
        return;
    m_widget->notify(event);
}

Gfx::IntSize AlignBox::calculate_preferred_size() const
{
    auto widget_width = calculate_preferred_widget_width();
    auto widget_height = calculate_preferred_widget_height();
    return { widget_width + abs(m_offset_x), widget_height + abs(m_offset_y) };
}

u32 AlignBox::calculate_preferred_widget_width() const
{
    auto box_width = m_layout_region.width();
    u32 widget_width = 0;

    switch (m_width_policy) {
    case SizePolicy::ExplicitLength:
        ASSERT(m_fixed_width.has_value());
        widget_width = m_fixed_width.value().to_pixels(box_width);
        break;
    case SizePolicy::Fill:
        widget_width = box_width;
        break;
    case SizePolicy::Minimum:
        if (!m_widget.is_valid())
            widget_width = 0;
        else if (m_widget->calculate_min_size_x().has_value())
            widget_width = m_widget->calculate_min_size_x().value();
        else
            widget_width = m_widget->calculate_preferred_size().width();
        break;
    default:
        ASSERT_NOT_REACHED;
    }

    // Clamp the widget width according to the min/max constraints and the box width.
    if (m_widget.is_valid()) {
        widget_width = clamp(
            widget_width,
            m_widget->calculate_min_size_x().value_or(0),
            m_widget->calculate_max_size_x().value_or(NumericLimits<u32>::max()));
    }

    return widget_width;
}

u32 AlignBox::calculate_preferred_widget_height() const
{
    auto box_height = m_layout_region.height();
    u32 widget_height = 0;

    switch (m_height_policy) {
    case SizePolicy::ExplicitLength:
        ASSERT(m_fixed_height.has_value());
        widget_height = m_fixed_height.value().to_pixels(box_height);
        break;
    case SizePolicy::Fill:
        widget_height = box_height;
        break;
    case SizePolicy::Minimum:
        if (!m_widget.is_valid())
            widget_height = 0;
        else if (m_widget->calculate_min_size_y().has_value())
            widget_height = m_widget->calculate_min_size_y().value();
        else
            widget_height = m_widget->calculate_preferred_size().height();
        break;
    default:
        ASSERT_NOT_REACHED;
    }

    // Clamp the widget width according to the min/max constraints and the box width.
    if (m_widget.is_valid()) {
        widget_height = clamp(
            widget_height,
            m_widget->calculate_min_size_y().value_or(0),
            m_widget->calculate_max_size_y().value_or(NumericLimits<u32>::max()));
    }

    return widget_height;
}

} // namespace GUI
