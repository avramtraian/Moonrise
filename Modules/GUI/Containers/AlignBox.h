/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <GUI/Forward.h>
#include <GUI/Length.h>
#include <GUI/SizePolicy.h>
#include <GUI/Widget.h>

namespace GUI {

enum class HAlignment {
    Left,
    Center,
    Right,
};

enum class VAlignment {
    Bottom,
    Center,
    Top,
};

enum class Alignment {
    BottomLeft,
    BottomCenter,
    BottomRight,
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
};

Alignment operator|(VAlignment, HAlignment);
Alignment operator|(HAlignment, VAlignment);

class AlignBox : public Widget {
    GUI_OBJECT(AlignBox, Widget);

public:
    void set_widget(NonnullRefPtr<Widget> const&);

    template<typename WidgetType, typename... Args>
    requires(is_derived_from<WidgetType, Widget>)
    NonnullRefPtr<WidgetType> set_widget(Args&&... args)
    {
        auto widget = WidgetType::construct(forward<Args>(args)...);
        set_widget(widget);
        return widget;
    }

    // NOTE: When providing SizePolicy::Preferred the default fixed size is set to 0.5 percentage.
    //       It is recommended that you use set_preferred_width() or set_preferred_height() to also
    //       explicitly initialize the size.
    void set_width_policy(SizePolicy);
    void set_height_policy(SizePolicy);

    void set_preferred_width(Length);
    void set_preferred_height(Length);

    void set_alignment(Alignment);
    void set_alignment(Gfx::FloatPoint);
    void set_child_anchor(Gfx::FloatPoint);
    void set_offset(s32, s32);

protected:
    virtual void notify(Event const&) override;
    virtual void on_layout_event(LayoutEvent const&) override;
    virtual void on_paint_event(PaintEvent const&) override;

    virtual Gfx::IntSize calculate_preferred_size() const override;
    u32 calculate_preferred_widget_width() const;
    u32 calculate_preferred_widget_height() const;

protected:
    RefPtr<Widget> m_widget;

    SizePolicy m_width_policy { SizePolicy::Fill };
    SizePolicy m_height_policy { SizePolicy::Fill };
    Optional<Length> m_fixed_width;
    Optional<Length> m_fixed_height;

    Gfx::FloatPoint m_alignment { 0.5F, 0.5F };
    Gfx::FloatPoint m_child_anchor { 0.5F, 0.5F };
    s32 m_offset_x { 0 };
    s32 m_offset_y { 0 };
};

} // namespace GUI
