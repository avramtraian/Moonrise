/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <Gfx/RenderDriver.h>

namespace Gfx {

class SoftwareRenderDriver : public RenderDriver {
    GFX_RENDER_DRIVER(SoftwareRenderDriver, RenderDriver);

public:
    virtual void initialize_impl() override;
    virtual void shutdown_impl() override;

    virtual RefPtr<PaintEngine> create_render_device() override;
    virtual RefPtr<Image> create_image(ImageFormat, IntSize) override;
};

} // namespace Gfx
