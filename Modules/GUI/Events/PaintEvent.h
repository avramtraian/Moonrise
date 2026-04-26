/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <GUI/Events/Event.h>
#include <Gfx/PaintBuffer.h>

namespace GUI {

class PaintEvent : public Event {
    GUI_EVENT(PaintEvent, paint, Event);

public:
    explicit PaintEvent(NonnullRefPtr<Gfx::PaintBuffer> const& paint_buffer)
        : m_paint_buffer(paint_buffer)
    {
    }

    NonnullRefPtr<Gfx::PaintBuffer> paint_buffer() const { return m_paint_buffer; }

private:
    NonnullRefPtr<Gfx::PaintBuffer> m_paint_buffer;
};

} // namespace GUI
