/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <GUI/Widget.h>
#include <Gfx/Border.h>
#include <Gfx/Color.h>
#include <Gfx/Forward.h>

namespace GUI {

class BorderBox : public Widget {
    GUI_OBJECT(BorderBox, Widget);

public:
    void set_widget(NonnullRefPtr<Widget> const&);

    template<typename WidgetType, typename... Arguments>
    requires(is_derived_from<WidgetType, Widget>)
    NonnullRefPtr<WidgetType> set_widget(Arguments&&... arguments)
    {
        auto widget = WidgetType::construct(forward<Arguments>(arguments)...);
        set_widget(widget);
        return widget;
    }

    void set_background(Gfx::Color);
    void set_color(Gfx::Color);
    void set_border(Gfx::IntBorder);

public:
    virtual bool needs_layout_update() const override { return m_widget->needs_layout_update(); }

    virtual void notify(Event const& event) override;
    virtual void on_layout_event(LayoutEvent const&) override;
    virtual void on_paint_event(PaintEvent const&) override;

    virtual Gfx::IntSize calculate_preferred_size() const override;

private:
    void paint_border(Gfx::Painter&);
    void paint_background(Gfx::Painter&);

protected:
    RefPtr<Widget> m_widget;
    Gfx::IntBorder m_border;
    Gfx::Color m_border_color;
    Gfx::Color m_background_color;
};

} // namespace GUI
