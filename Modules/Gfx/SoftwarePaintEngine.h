/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <Gfx/PaintEngine.h>
#include <Gfx/SoftwareImage.h>

namespace Gfx {

class SoftwarePaintEngine final : public PaintEngine {
public:
    SoftwarePaintEngine() = default;
    virtual ~SoftwarePaintEngine() override = default;

public:
    virtual void initialize() override;
    virtual void shutdown() override;

    virtual void begin_frame() override;
    virtual void end_frame() override;
    virtual void execute_paint_buffer(NonnullRefPtr<PaintBuffer> const&) override;

    virtual void set_render_target(NonnullRefPtr<Image> const&) override;

private:
    static void execute_quad_draw_command(Bitmap&, QuadDrawCommand const&);

private:
    RefPtr<SoftwareImage> m_render_target;
};

} // namespace Gfx
