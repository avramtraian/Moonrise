/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/OwnPtr.h>
#include <AND/RefPtr.h>
#include <AND/Utf8View.h>
#include <Gfx/Forward.h>
#include <Gfx/ImageFormat.h>
#include <Gfx/Size.h>

namespace Gfx {

enum class RenderDriverType {
    Unknown = 0,
    Software,
};

class RenderDriver {
    AND_MAKE_NONCOPYABLE(RenderDriver);
    AND_MAKE_NONMOVABLE(RenderDriver);

public:
    static void initialize(RenderDriverType);
    static void shutdown();
    static RenderDriver& the();

    RenderDriver() = default;
    virtual ~RenderDriver() = default;
    virtual Utf8View class_name() const = 0;

public:
    virtual void initialize_impl() = 0;
    virtual void shutdown_impl() = 0;

    virtual RefPtr<PaintEngine> create_render_device() = 0;
    virtual RefPtr<Image> create_image(ImageFormat, IntSize) = 0;
};

#define GFX_RENDER_DRIVER(type, base_type) \
public:                                    \
    using Base = base_type;                \
    type() = default;                      \
    virtual ~type() override = default;    \
    virtual ::AND::Utf8View class_name() const override { return VIEW(#type); }

} // namespace Gfx
