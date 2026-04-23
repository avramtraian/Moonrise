/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <GUI/Forward.h>
#include <GUI/Object.h>
#include <Gfx/Rect.h>

namespace GUI {

class Widget : public Object {
    GUI_OBJECT(Widget, Object);

public:
    virtual void on_layout_event(LayoutEvent const&);
    virtual void on_paint_event(PaintEvent const&);

    virtual void on_mouse_moved_event(MouseEvent const&);

public:
    virtual Optional<u32> calculate_min_size_x() const { return {}; }
    virtual Optional<u32> calculate_max_size_x() const { return {}; }

    virtual Optional<u32> calculate_min_size_y() const { return {}; }
    virtual Optional<u32> calculate_max_size_y() const { return {}; }

private:
    Gfx::IntRect m_layout_region;
};

} // namespace GUI
