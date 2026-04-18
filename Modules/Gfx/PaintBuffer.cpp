/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <Gfx/PaintBuffer.h>

namespace Gfx {

void PaintBuffer::clear()
{
    m_draw_commands.clear();
}

void PaintBuffer::draw_quad(IntRect rect, Color color)
{
    auto& command = m_draw_commands.emplace_back();
    command.rect = rect;
    command.color = color;
}

ConstSpan<QuadDrawCommand> PaintBuffer::draw_commands() const
{
    return m_draw_commands.span();
}

} // namespace Gfx
