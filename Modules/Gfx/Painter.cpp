/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/MathUtilities.h>
#include <Gfx/Painter.h>

namespace Gfx {

Painter::Painter(NonnullRefPtr<PaintBuffer> const& paint_buffer, IntRect clip_region)
    : m_paint_buffer(paint_buffer)
    , m_clip_region(clip_region)
{
}

void Painter::fill_rect(IntRect rect, Color color)
{
    auto clipped_rect = IntRect::intersect(rect, m_clip_region);
    m_paint_buffer->draw_quad(clipped_rect, color);
}

void Painter::outline_rect(IntRect rect, Color color, IntBorder border)
{
    fill_rect(rect.bottom_border(border), color);
    fill_rect(rect.top_border(border), color);
    fill_rect(rect.left_border(border), color);
    fill_rect(rect.right_border(border), color);
}

} // namespace Gfx
