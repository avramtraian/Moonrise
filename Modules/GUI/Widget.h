/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <GUI/Events/MouseEvent.h>
#include <GUI/Forward.h>
#include <GUI/Object.h>
#include <Gfx/Rect.h>

namespace GUI {

class Widget : public Object {
    GUI_ABSTRACT_OBJECT(Widget, Object);

public:
    virtual void notify(Event const& event);

public:
    void schedule_layout_event() { m_needs_layout_update = true; }
    void schedule_paint_event() { m_needs_paint_update = true; }

    Gfx::IntRect layout_region() const { return m_layout_region; }
    virtual bool needs_layout_update() const { return m_needs_layout_update; }
    virtual bool needs_paint_update() const { return m_needs_paint_update; }

    virtual Gfx::IntSize calculate_preferred_size() const = 0;
    virtual Optional<u32> calculate_min_size_x() const { return {}; }
    virtual Optional<u32> calculate_max_size_x() const { return {}; }
    virtual Optional<u32> calculate_min_size_y() const { return {}; }
    virtual Optional<u32> calculate_max_size_y() const { return {}; }

protected:
    virtual void on_unhandled_event(Event const&);
    virtual void on_layout_event(LayoutEvent const&);
    virtual void on_paint_event(PaintEvent const&);
    virtual void on_mouse_event(MouseEvent const&);
    virtual void on_mouse_moved_event(MouseEvent const&);
    virtual void on_mouse_button_pressed_event(MouseEvent const&);
    virtual void on_mouse_button_released_event(MouseEvent const&);
    virtual void on_mouse_wheel_scrolled_event(MouseEvent const&);

protected:
    Gfx::IntRect m_layout_region;

    bool m_needs_layout_update { false };
    bool m_needs_paint_update { false };
};

} // namespace GUI
