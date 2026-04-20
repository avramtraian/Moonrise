/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <Gfx/PaintBuffer.h>
#include <Gfx/RenderDriver.h>
#include <Gfx/SoftwarePaintEngine.h>

namespace Gfx {

void SoftwarePaintEngine::initialize()
{
}

void SoftwarePaintEngine::shutdown()
{
}

void SoftwarePaintEngine::begin_frame()
{
}

void SoftwarePaintEngine::end_frame()
{
}

void SoftwarePaintEngine::execute_paint_buffer(NonnullRefPtr<PaintBuffer> const& paint_buffer)
{
    ASSERT(m_render_target.is_valid());

    auto& bitmap = *m_render_target->bitmap();
    auto draw_commands = paint_buffer->draw_commands();
    for (QuadDrawCommand const& command : draw_commands)
        execute_quad_draw_command(bitmap, command);
}

void SoftwarePaintEngine::set_render_target(NonnullRefPtr<Image> const& new_render_target)
{
    m_render_target = new_render_target.as<SoftwareImage>();
}

void SoftwarePaintEngine::execute_quad_draw_command(Bitmap& bitmap, QuadDrawCommand const& command)
{
    IntRect region = IntRect::intersect(command.rect, bitmap.rect());
    s32 min_y = region.min_point().y;
    s32 min_x = region.min_point().x;
    s32 max_y = region.max_point().y;

    for (s32 y = min_y; y < max_y; y++)
        bitmap.set_pixels_in_row(min_x, y, region.width(), command.color);
}

} // namespace Gfx
