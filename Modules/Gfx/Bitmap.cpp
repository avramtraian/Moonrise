/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <Gfx/Bitmap.h>

namespace Gfx {

usize bytes_per_pixel(BitmapFormat format)
{
    switch (format) {
    case BitmapFormat::Unknown:
        return 0;
    case BitmapFormat::RGBA_8888:
    case BitmapFormat::BGRA_8888:
        return 4;
    default:
        ASSERT_NOT_REACHED;
    }
}

usize pitch_for_format(BitmapFormat format, u32 width)
{
    usize bpp = bytes_per_pixel(format);
    // FIXME: We should check that the following multiplication does not overflow!
    return width * bpp;
}

Bitmap::Bitmap(ByteBuffer buffer, BitmapFormat format, IntSize size, usize pitch, usize number_of_rows)
    : m_buffer(move(buffer))
    , m_format(format)
    , m_size(size)
    , m_pitch(pitch)
    , m_number_of_rows(number_of_rows)
{
}

Bitmap::~Bitmap()
{
}

RefPtr<Bitmap> Bitmap::allocate(BitmapFormat format, IntSize size)
{
    usize pitch = pitch_for_format(format, size.width());
    usize number_of_rows = size.height();
    return Bitmap::allocate_aligned(format, size, pitch, number_of_rows);
}

RefPtr<Bitmap> Bitmap::allocate_aligned(BitmapFormat format, IntSize size, usize pitch, usize number_of_rows)
{
    ASSERT(size.width() <= pitch);
    ASSERT(size.height() <= number_of_rows);

    // FIXME: We should check that the following multiplication does not overflow!
    usize buffer_size = pitch * number_of_rows;
    ByteBuffer buffer = ByteBuffer::allocate(buffer_size);

    return adopt(new Bitmap(move(buffer), format, size, pitch, number_of_rows));
}

void Bitmap::set_pixel(u32 x, u32 y, Color color)
{
    ASSERT(x < m_size.width());
    ASSERT(y < m_size.height());

    u32 packed_color = 0;
    switch (m_format) {
    case BitmapFormat::RGBA_8888:
        packed_color = color.pack_to_rgba();
        break;
    case BitmapFormat::BGRA_8888:
        packed_color = color.pack_to_bgra();
        break;
    default:
        ASSERT_NOT_REACHED;
    }

    usize bpp = bytes_per_pixel();
    ASSERT(bpp == 4);
    u32* pixel_ptr = reinterpret_cast<u32*>(m_buffer.bytes() + (y * m_pitch) + (x * bpp));
    *pixel_ptr = packed_color;
}

void Bitmap::set_pixels_in_row(u32 x, u32 y, u32 number_of_pixels, Color color)
{
    ASSERT(x < m_size.width());
    ASSERT(y < m_size.height());
    ASSERT(x + number_of_pixels <= m_size.width());

    u32 packed_color = 0;
    switch (m_format) {
    case BitmapFormat::RGBA_8888:
        packed_color = color.pack_to_rgba();
        break;
    case BitmapFormat::BGRA_8888:
        packed_color = color.pack_to_bgra();
        break;
    default:
        ASSERT_NOT_REACHED;
    }

    usize bpp = bytes_per_pixel();
    ASSERT(bpp == 4);
    u32* pixel_ptr = reinterpret_cast<u32*>(m_buffer.bytes() + (y * m_pitch) + (x * bpp));
    for (u32 i = 0; i < number_of_pixels; ++i)
        *pixel_ptr++ = packed_color;
}

} // namespace Gfx
