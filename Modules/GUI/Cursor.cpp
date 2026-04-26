/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/StringBuilder.h>
#include <GUI/Cursor.h>

namespace GUI {

bool Cursor::is_down(MouseButton button) const
{
    ASSERT(button < MouseButton::MaxEnumCount);
    return m_button_is_down[to_underlying(button)];
}

void Cursor::set_absolute_position(Gfx::IntPoint absolute_position, Badge<Application>)
{
    m_absolute_position = absolute_position;
}

void Cursor::set_is_down(MouseButton button, bool value, Badge<Application>)
{
    ASSERT(button < MouseButton::MaxEnumCount);
    m_button_is_down[to_underlying(button)] = value;
}

void append_to_builder(StringBuilder& builder, Optional<StringView> const&, MouseButton const& button)
{
    switch (button) {
    case MouseButton::Unknown:
        builder.append(VIEW("Unknown"));
        break;
    case MouseButton::Left:
        builder.append(VIEW("Left"));
        break;
    case MouseButton::Right:
        builder.append(VIEW("Right"));
        break;
    case MouseButton::Middle:
        builder.append(VIEW("Middle"));
        break;
    default:
        builder.append_formatted(VIEW("MouseButton({})"), to_underlying(button));
        break;
    }
}

} // namespace GUI
