/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <Gfx/RenderDriver.h>
#include <Gfx/SoftwareRenderDriver.h>

namespace Gfx {

static RenderDriver* s_the = nullptr;

void RenderDriver::initialize(RenderDriverType type)
{
    ASSERT(!s_the);

    switch (type) {
    case RenderDriverType::Software:
        s_the = new SoftwareRenderDriver();
        break;
    default:
        ASSERT_NOT_REACHED;
    }

    ASSERT(s_the);
    s_the->initialize_impl();
}

void RenderDriver::shutdown()
{
    ASSERT(s_the);
    s_the->shutdown_impl();
    delete s_the;
}

RenderDriver& RenderDriver::the()
{
    ASSERT(s_the);
    return *s_the;
}

} // namespace Gfx
