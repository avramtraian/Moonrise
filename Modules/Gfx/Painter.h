/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefPtr.h>
#include <Gfx/Color.h>
#include <Gfx/PaintBuffer.h>
#include <Gfx/Rect.h>

namespace Gfx {

class Painter {
public:
    Painter(NonnullRefPtr<PaintBuffer> const&, IntRect clip_region);

    void fill_rect(IntRect, Color);
    void outline_rect(IntRect, Color, u32 border_thickness);

private:
    NonnullRefPtr<PaintBuffer> m_paint_buffer;
    IntRect m_clip_region;
};

} // namespace Gfx
