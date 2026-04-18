/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <Gfx/Bitmap.h>
#include <Gfx/Image.h>

namespace Gfx {

class SoftwareImage : public Image {
    GFX_IMAGE(SoftwareImage, Image);

public:
    virtual void initialize(ImageFormat, IntSize) override;

    virtual ImageFormat format() const override;
    virtual IntSize size() const override;

    RefPtr<Bitmap> bitmap() const { return m_bitmap; }

private:
    RefPtr<Bitmap> m_bitmap;
};

} // namespace Gfx
