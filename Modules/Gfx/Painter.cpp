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

void Painter::outline_rect(IntRect rect, Color color, u32 border_thickness)
{
    border_thickness = clamp<s32>(border_thickness, 0, divide_int_up<u32>(rect.width(), 2));
    border_thickness = clamp<s32>(border_thickness, 0, divide_int_up<u32>(rect.height(), 2));

    IntRect bottom;
    bottom.set_offset(rect.offset());
    bottom.set_size(rect.width(), border_thickness);
    fill_rect(bottom, color);

    IntRect top;
    top.set_offset(rect.offset_x(), rect.max_point().y - border_thickness);
    top.set_size(rect.width(), border_thickness);
    fill_rect(top, color);

    IntRect left;
    left.set_offset(rect.offset_x(), rect.offset_y() + border_thickness);
    left.set_size(border_thickness, rect.height() - 2 * border_thickness);
    fill_rect(left, color);

    IntRect right;
    right.set_offset(rect.max_point().x - border_thickness, rect.offset_y() + border_thickness);
    right.set_size(border_thickness, rect.height() - 2 * border_thickness);
    fill_rect(right, color);
}

} // namespace Gfx
