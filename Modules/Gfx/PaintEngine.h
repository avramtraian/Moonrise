/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefPtr.h>
#include <Gfx/Forward.h>
#include <Gfx/Image.h>
#include <Gfx/Size.h>

namespace Gfx {

class PaintEngine : public RefCounted {
public:
    PaintEngine() = default;
    virtual ~PaintEngine() = default;

public:
    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    virtual void begin_frame() = 0;
    virtual void end_frame() = 0;
    virtual void execute_paint_buffer(NonnullRefPtr<PaintBuffer> const&) = 0;

    virtual void set_render_target(NonnullRefPtr<Image> const&) = 0;
};

} // namespace Gfx
