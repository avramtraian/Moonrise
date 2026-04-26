/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Cursor.h>
#include <GUI/Events/MouseEvent.h>
#include <GUI/Platform/WindowManager.h>
#include <GUI/Window.h>

namespace GUI {

MouseEvent::MouseEvent(Type type, NonnullRefPtr<Cursor> const& cursor, NonnullRefPtr<Window> const& window)
    : m_cursor(cursor)
    , m_window(window)
    , m_type(type)
{
    // Find the cursor position relative to the source window.
    auto maybe_position = WindowManager::the().calculate_relative_position(window->native_handle(), cursor->absolute_position());
    ASSERT(maybe_position.has_value());
    m_position = maybe_position.value();
}

} // namespace GUI
