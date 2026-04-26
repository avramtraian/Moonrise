/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <GUI/Events/Event.h>
#include <Gfx/Rect.h>

namespace GUI {

class LayoutEvent : public Event {
    GUI_EVENT(LayoutEvent, layout, Event);

public:
    explicit LayoutEvent(Gfx::IntRect region)
        : m_region(region)
    {
    }

    Gfx::IntRect const& region() const { return m_region; }

private:
    Gfx::IntRect m_region;
};

} // namespace GUI
