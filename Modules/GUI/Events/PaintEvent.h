/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <Gfx/PaintBuffer.h>

namespace GUI {

class PaintEvent {
public:
    PaintEvent(NonnullRefPtr<Gfx::PaintBuffer> const& paint_buffer, Gfx::IntRect region)
        : m_paint_buffer(paint_buffer)
        , m_region(region)
    {
    }

    NonnullRefPtr<Gfx::PaintBuffer> paint_buffer() const { return m_paint_buffer; }
    Gfx::IntRect region() const { return m_region; }

private:
    NonnullRefPtr<Gfx::PaintBuffer> m_paint_buffer;
    Gfx::IntRect m_region;
};

} // namespace GUI
