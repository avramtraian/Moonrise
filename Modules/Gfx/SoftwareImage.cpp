/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <Gfx/SoftwareImage.h>

namespace Gfx {

void SoftwareImage::initialize(ImageFormat format, IntSize size)
{
    ASSERT(size.width() > 0);
    ASSERT(size.height() > 0);

    BitmapFormat bitmap_format = BitmapFormat::Unknown;
    switch (format) {
    case ImageFormat::RGBA_8888:
        bitmap_format = BitmapFormat::RGBA_8888;
        break;
    case ImageFormat::BGRA_8888:
        bitmap_format = BitmapFormat::BGRA_8888;
        break;
    default:
        ASSERT_NOT_REACHED;
    }

    m_bitmap = Bitmap::allocate(bitmap_format, size);
}

ImageFormat SoftwareImage::format() const
{
    ASSERT(m_bitmap.is_valid());
    switch (m_bitmap->format()) {
    case BitmapFormat::RGBA_8888:
        return ImageFormat::RGBA_8888;
    case BitmapFormat::BGRA_8888:
        return ImageFormat::BGRA_8888;
    default:
        ASSERT_NOT_REACHED;
    }
}

IntSize SoftwareImage::size() const
{
    ASSERT(m_bitmap.is_valid());
    return m_bitmap->size();
}

} // namespace Gfx
