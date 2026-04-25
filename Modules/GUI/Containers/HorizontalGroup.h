/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefPtr.h>
#include <AND/Vector.h>
#include <GUI/Length.h>
#include <GUI/SizePolicy.h>
#include <GUI/Widget.h>

namespace GUI {

// NOTE: The alignment matters only when there is no element in the group marked with HGroupSizePolicy::Fill,
//       as that would cause that widget to occupy all available space, and thus alignment would be irrelevant.
enum class HGroupAlignment {
    Left,
    Center,
    Right,
    Justify,
};

class HorizontalGroup : public Widget {
    GUI_OBJECT(HorizontalGroup, Widget);

    struct GroupElement {
        explicit GroupElement(NonnullRefPtr<Widget> const& widget)
            : widget(widget)
        {
        }

        NonnullRefPtr<Widget> widget;
        SizePolicy size_policy { SizePolicy::Fill };
        Optional<Length> preferred_width;
    };

public:
    void add_widget(NonnullRefPtr<Widget> const&);
    RefPtr<Widget> query_widget(u32 index);

    template<typename WidgetType, typename... Args>
    requires(is_derived_from<WidgetType, Widget>)
    NonnullRefPtr<WidgetType> add_widget(Args&&... args)
    {
        auto widget = WidgetType::construct(forward<Args>(args)...);
        add_widget(widget);
        return widget;
    }

    void set_alignment(HGroupAlignment);

    void set_widget_size_policy(NonnullRefPtr<Widget> const&, SizePolicy);
    void set_widget_preferred_width(NonnullRefPtr<Widget> const&, Length);

private:
    virtual void on_layout_event(LayoutEvent const&) override;
    virtual void on_paint_event(PaintEvent const&) override;

    virtual Gfx::IntSize calculate_preferred_size() const override;
    virtual Optional<u32> calculate_min_size_x() const override;
    virtual Optional<u32> calculate_max_size_x() const override;
    virtual Optional<u32> calculate_min_size_y() const override;
    virtual Optional<u32> calculate_max_size_y() const override;

    Optional<GroupElement&> find_element(NonnullRefPtr<Widget> const&);
    Optional<GroupElement const&> find_element(NonnullRefPtr<Widget> const&) const;

private:
    Vector<GroupElement> m_elements;
    HGroupAlignment m_alignment { HGroupAlignment::Center };
};

} // namespace GUI
