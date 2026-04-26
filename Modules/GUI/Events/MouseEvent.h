/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Badge.h>
#include <AND/RefPtr.h>
#include <GUI/Cursor.h>
#include <GUI/Forward.h>

namespace GUI {

class MouseEvent {
public:
    MouseEvent(NonnullRefPtr<Cursor> const&, NonnullRefPtr<Window> const&);
    ~MouseEvent();

    Cursor& cursor() const { return *m_cursor; }
    Window& window() const { return *m_window; }

    Gfx::IntPoint position() const { return m_position; }
    MouseButton pressed_button() const { return m_pressed_button.value_or(MouseButton::Unknown); }
    MouseButton released_button() const { return m_released_button.value_or(MouseButton::Unknown); }
    float scroll_delta_x() const { return m_scroll_delta_x.value_or(0.0F); }
    float scroll_delta_y() const { return m_scroll_delta_y.value_or(0.0F); }

public:
    void set_pressed_button(MouseButton button, Badge<Window>) { m_pressed_button = button; }
    void set_released_button(MouseButton button, Badge<Window>) { m_released_button = button; }
    void set_scroll_delta(float delta_x, float delta_y, Badge<Window>)
    {
        m_scroll_delta_x = delta_x;
        m_scroll_delta_y = delta_y;
    }

private:
    NonnullRefPtr<Cursor> m_cursor;
    NonnullRefPtr<Window> m_window;
    Gfx::IntPoint m_position;
    Optional<MouseButton> m_pressed_button;
    Optional<MouseButton> m_released_button;
    Optional<float> m_scroll_delta_x;
    Optional<float> m_scroll_delta_y;
};

} // namespace GUI
