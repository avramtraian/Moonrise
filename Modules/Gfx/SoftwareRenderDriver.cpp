/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <Gfx/SoftwareImage.h>
#include <Gfx/SoftwarePaintEngine.h>
#include <Gfx/SoftwareRenderDriver.h>

namespace Gfx {

void SoftwareRenderDriver::initialize_impl()
{
}

void SoftwareRenderDriver::shutdown_impl()
{
}

RefPtr<PaintEngine> SoftwareRenderDriver::create_render_device()
{
    auto render_device = adopt(new SoftwarePaintEngine());
    render_device->initialize();
    return render_device;
}

RefPtr<Image> SoftwareRenderDriver::create_image(ImageFormat format, IntSize size)
{
    auto image = adopt(new SoftwareImage());
    image->initialize(format, size);
    return image;
}

} // namespace Gfx
