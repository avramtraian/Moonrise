/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/RefCounted.h>
#include <AND/Utf8View.h>
#include <Gfx/ImageFormat.h>
#include <Gfx/Rect.h>

namespace Gfx {

class Image : public RefCounted {
public:
    Image() = default;
    virtual ~Image() override = default;
    virtual Utf8View class_name() const = 0;

public:
    virtual void initialize(ImageFormat, IntSize) = 0;

    virtual ImageFormat format() const = 0;
    virtual IntSize size() const = 0;
    IntRect rect() const { return { IntPoint::zero(), size() }; }
};

#define GFX_IMAGE(type, base_type)      \
public:                                 \
    using Base = base_type;             \
    type() = default;                   \
    virtual ~type() override = default; \
    virtual ::AND::Utf8View class_name() const override { return VIEW(#type); }

} // namespace Gfx
