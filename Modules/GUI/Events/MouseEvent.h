/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Badge.h>
#include <GUI/Forward.h>
#include <Gfx/Rect.h>

namespace GUI {

enum class MouseButton : u8 {
    Unknown = 0,
    Left,
    Right,
    Middle,
    MaxEnumCount,
};

enum class MouseButtonAction {
    Pressed,
    Released,
};

struct MouseButtonState {
    bool is_down { false };
    bool was_pressed_this_frame { false };
    bool was_released_this_frame { false };
};

class MouseEvent {
public:
    static MouseEvent clone(MouseEvent const& source, Gfx::IntRect parent_region, Gfx::IntRect widget_region)
    {
        MouseEvent cloned = source;
        auto offset_x = widget_region.offset_x() - parent_region.offset_x();
        auto offset_y = widget_region.offset_y() - parent_region.offset_y();
        cloned.m_relative_position.move_by(-offset_x, -offset_y);
        return cloned;
    }

public:
    bool is_down(MouseButton button) const { return m_button_states[to_underlying(button)].is_down; }
    bool is_up(MouseButton button) const { return !is_down(button); }
    bool was_pressed_this_frame(MouseButton button) const { return m_button_states[to_underlying(button)].was_pressed_this_frame; }
    bool was_released_this_frame(MouseButton button) const { return m_button_states[to_underlying(button)].was_released_this_frame; }
    Gfx::IntPoint position() const { return m_relative_position; }

    void set_relative_position(Gfx::IntPoint position, Badge<Window>) { m_relative_position = position; }
    MouseButtonState& button_state(MouseButton button, Badge<Window>) { return m_button_states[to_underlying(button)]; }

private:
    MouseButtonState m_button_states[to_underlying(MouseButton::MaxEnumCount)];
    Gfx::IntPoint m_relative_position;
};

} // namespace GUI
