/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Containers/HorizontalGroup.h>
#include <GUI/Events/LayoutEvent.h>
#include <GUI/Events/PaintEvent.h>

namespace GUI {

void HorizontalGroup::add_widget(NonnullRefPtr<Widget> const& widget)
{
    m_elements.emplace_back(widget);
    schedule_layout_event();
    schedule_paint_event();
}

RefPtr<Widget> HorizontalGroup::query_widget(u32 index)
{
    if (index >= m_elements.count())
        return {};
    return m_elements[index].widget;
}

void HorizontalGroup::set_alignment(HGroupAlignment alignment)
{
    auto old_alignment = m_alignment;
    m_alignment = alignment;
    if (old_alignment != m_alignment) {
        schedule_layout_event();
        schedule_paint_event();
    }
}

void HorizontalGroup::set_widget_size_policy(NonnullRefPtr<Widget> const& widget, SizePolicy size_policy)
{
    auto maybe_element = find_element(widget);
    if (!maybe_element.has_value())
        return;
    auto& element = maybe_element.value();

    if (element.size_policy != size_policy) {
        element.size_policy = size_policy;
        if (element.size_policy == SizePolicy::ExplicitLength)
            element.preferred_width = Length::from_percentage(0.5F);
        else
            element.preferred_width.release();

        schedule_layout_event();
        schedule_paint_event();
    }
}

void HorizontalGroup::set_widget_preferred_width(NonnullRefPtr<Widget> const& widget, Length preferred_width)
{
    auto maybe_element = find_element(widget);
    if (!maybe_element.has_value())
        return;
    auto& element = maybe_element.value();

    if (element.size_policy == SizePolicy::ExplicitLength) {
        ASSERT(element.preferred_width.has_value());
        if (element.preferred_width.value() == preferred_width)
            return;
    }

    element.size_policy = SizePolicy::ExplicitLength;
    element.preferred_width = preferred_width;
}

void HorizontalGroup::notify(Event const& event)
{
    if (m_elements.is_empty())
        return Base::notify(event);

    // If the event is not a paint or layout event, we should forward the event handling to the child widgets.
    if (!event.is_layout_event() && !event.is_paint_event() && !event.is_mouse_event()) {
        for (auto& element : m_elements)
            element.widget->notify(event);
        return;
    }

    if (event.is_mouse_event()) {
        // Forward the event to the child widget that contains the mouse position in its layout region.
        auto const& mouse_event = event.as_mouse_event();
        for (auto& element : m_elements) {
            if (element.widget->layout_region().contains(mouse_event.position())) {
                element.widget->notify(mouse_event);
                break;
            }
        }
        return;
    }

    Base::notify(event);
}

void HorizontalGroup::on_layout_event(LayoutEvent const& event)
{
    Base::on_layout_event(event);
    if (m_elements.is_empty())
        return;

    Vector<Gfx::IntRect, 8> widget_regions;
    widget_regions.ensure_capacity(m_elements.count());

    s32 remining_width = m_layout_region.width();
    u32 elements_marked_for_fill = 0;

    // Calculate the width of widgets that are not marked with SizePolicy::Fill.
    //--------------------------------------------------------------------------
    for (auto& element : m_elements) {
        Gfx::IntRect& widget_region = widget_regions.emplace_back();
        widget_region.set_height(m_layout_region.height());

        switch (element.size_policy) {
        case SizePolicy::ExplicitLength:
            ASSERT(element.preferred_width.has_value());
            widget_region.set_width(element.preferred_width.value().to_pixels(m_layout_region.width()));
            remining_width -= widget_region.width();
            break;

        case SizePolicy::Minimum:
            if (element.widget->calculate_min_size_x().has_value())
                widget_region.set_width(element.widget->calculate_min_size_x().value());
            else
                widget_region.set_width(element.widget->calculate_preferred_size().width());
            remining_width -= widget_region.width();
            break;

        case SizePolicy::Fill:
            elements_marked_for_fill++;
            break;

        default:
            ASSERT_NOT_REACHED;
        }
    }

    // Set the width of widgets marked with SizePolicy::Fill.
    //--------------------------------------------------------------------------
    if (remining_width > 0 && elements_marked_for_fill > 0) {
        u32 width_per_fill_element = static_cast<u32>(remining_width) / elements_marked_for_fill;
        u32 additional_pixels = static_cast<u32>(remining_width) % elements_marked_for_fill;
        for (size_t index = 0; index < m_elements.count(); index++) {
            auto& element = m_elements[index];
            auto& widget_region = widget_regions[index];
            if (element.size_policy == SizePolicy::Fill)
                widget_region.set_width(width_per_fill_element);
            if (index < additional_pixels)
                widget_region.set_width(widget_region.width() + 1);
        }
    }

    // Set the offset of each widget region.
    //--------------------------------------------------------------------------
    Gfx::IntPoint next_widget_offset = m_layout_region.offset();
    for (auto& widget_region : widget_regions) {
        widget_region.set_offset(next_widget_offset);
        widget_region = Gfx::IntRect::intersect(widget_region, m_layout_region);
        next_widget_offset.x += widget_region.width();
    }

    // Layout each widget.
    //--------------------------------------------------------------------------
    for (size_t index = 0; index < m_elements.count(); index++) {
        auto& element = m_elements[index];
        auto& widget_region = widget_regions[index];
        LayoutEvent layout_event { widget_region };
        element.widget->notify(layout_event);
    }
}

void HorizontalGroup::on_paint_event(PaintEvent const& event)
{
    Base::on_paint_event(event);
    for (auto& element : m_elements)
        element.widget->notify(event);
}

Gfx::IntSize HorizontalGroup::calculate_preferred_size() const
{
    Gfx::IntSize preferred_size { 0, 0 };
    for (auto const& element : m_elements) {
        auto widget_preferred_size = element.widget->calculate_preferred_size();
        preferred_size.x += widget_preferred_size.x;
        // FIXME: Is it the best idea to set the preferred height as the maximum of the preferred heights of the child widgets?
        preferred_size.y = max(preferred_size.y, widget_preferred_size.y);
    }
    return preferred_size;
}

Optional<u32> HorizontalGroup::calculate_min_size_x() const
{
    u32 min_size = 0;
    bool has_min_size = false;

    for (auto const& element : m_elements) {
        auto widget_min_size = element.widget->calculate_min_size_x();
        if (!widget_min_size.has_value())
            continue;
        min_size += widget_min_size.value();
        has_min_size = true;
    }

    if (!has_min_size)
        return {};
    return min_size;
}

Optional<u32> HorizontalGroup::calculate_max_size_x() const
{
    if (m_elements.is_empty())
        return {};

    u32 max_size = 0;
    for (auto const& element : m_elements) {
        auto widget_max_size = element.widget->calculate_max_size_x();
        if (!widget_max_size.has_value())
            return {};
        max_size += widget_max_size.value();
    }
    return max_size;
}

Optional<u32> HorizontalGroup::calculate_min_size_y() const
{
    Optional<u32> min_size;
    for (auto const& element : m_elements) {
        auto widget_min_size = element.widget->calculate_min_size_y();
        if (!widget_min_size.has_value())
            continue;
        if (!min_size.has_value() || widget_min_size.value() > min_size.value())
            min_size = widget_min_size.value();
    }
    return min_size;
}

Optional<u32> HorizontalGroup::calculate_max_size_y() const
{
    Optional<u32> max_size;
    for (auto const& element : m_elements) {
        auto widget_max_size = element.widget->calculate_max_size_y();
        if (!widget_max_size.has_value())
            return {};
        if (!max_size.has_value() || widget_max_size.value() < max_size.value())
            max_size = widget_max_size.value();
    }
    return max_size;
}

Optional<HorizontalGroup::GroupElement&> HorizontalGroup::find_element(NonnullRefPtr<Widget> const& widget)
{
    for (auto& element : m_elements) {
        if (element.widget.get() == widget.get())
            return element;
    }
    return {};
}

Optional<HorizontalGroup::GroupElement const&> HorizontalGroup::find_element(NonnullRefPtr<Widget> const& widget) const
{
    for (auto const& element : m_elements) {
        if (element.widget.get() == widget.get())
            return element;
    }
    return {};
}

} // namespace GUI
