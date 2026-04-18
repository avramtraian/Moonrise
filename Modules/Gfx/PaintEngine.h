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

enum class RenderDeviceType {
    Unknown = 0,
    Software,
};

class PaintEngine : public RefCounted {
public:
    static RefPtr<PaintEngine> create(RenderDeviceType);
    virtual ~PaintEngine() = default;

public:
    virtual void initialize() = 0;
    virtual void shutdown() = 0;

    virtual void begin_frame() = 0;
    virtual void end_frame() = 0;
    virtual void execute_paint_buffer(NonnullRefPtr<PaintBuffer> const&) = 0;

    virtual void resize_images(IntSize) = 0;
    virtual NonnullRefPtr<Image> get_current_image() = 0;
};

} // namespace Gfx
