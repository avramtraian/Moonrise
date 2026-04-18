/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefPtr.h>
#include <AND/Vector.h>
#include <Gfx/Color.h>
#include <Gfx/Rect.h>

namespace Gfx {

struct QuadDrawCommand {
    IntRect rect;
    Color color;
};

class PaintBuffer : public RefCounted {
public:
    PaintBuffer() = default;
    ~PaintBuffer() = default;

    void clear();
    void draw_quad(IntRect, Color);
    ConstSpan<QuadDrawCommand> draw_commands() const;

private:
    Vector<QuadDrawCommand> m_draw_commands;
};

} // namespace Gfx
