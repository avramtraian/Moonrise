/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Badge.h>
#include <AND/Forward.h>
#include <GUI/Forward.h>
#include <GUI/Object.h>
#include <Gfx/Point.h>

namespace GUI {

enum class MouseButton : u8 {
    Unknown = 0,
    Left,
    Right,
    Middle,
    MaxEnumCount,
};

class Cursor : public Object {
    GUI_OBJECT(Cursor, Object);

public:
    Gfx::IntPoint absolute_position() const { return m_absolute_position; }
    bool is_down(MouseButton) const;

    void set_absolute_position(Gfx::IntPoint, Badge<Application>);
    void set_is_down(MouseButton, bool, Badge<Application>);

public:
    Gfx::IntPoint m_absolute_position;
    bool m_button_is_down[to_underlying(MouseButton::MaxEnumCount)] {};
};

void append_to_builder(StringBuilder&, Optional<StringView> const&, MouseButton const&);

} // namespace GUI
