/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/ByteBuffer.h>
#include <AND/RefPtr.h>
#include <Gfx/Color.h>
#include <Gfx/Rect.h>

namespace Gfx {

enum class BitmapFormat {
    Unknown = 0,
    RGBA_8888,
    BGRA_8888,
};

usize bytes_per_pixel(BitmapFormat);
usize pitch_for_format(BitmapFormat, u32 width);

class Bitmap : public RefCounted {
public:
    static RefPtr<Bitmap> allocate(BitmapFormat, IntSize);
    static RefPtr<Bitmap> allocate_aligned(BitmapFormat, IntSize, usize pitch, usize number_of_rows);

public:
    ~Bitmap();

    NODISCARD ALWAYS_INLINE ByteBuffer& buffer() { return m_buffer; }
    NODISCARD ALWAYS_INLINE ByteBuffer const& buffer() const { return m_buffer; }
    NODISCARD ALWAYS_INLINE RWByteSpan rw_byte_span() { return m_buffer.byte_span(); }
    NODISCARD ALWAYS_INLINE ROByteSpan ro_byte_span() const { return m_buffer.ro_byte_span(); }

    NODISCARD ALWAYS_INLINE BitmapFormat format() const { return m_format; }
    NODISCARD ALWAYS_INLINE IntSize size() const { return m_size; }
    NODISCARD ALWAYS_INLINE u32 width() const { return m_size.width(); }
    NODISCARD ALWAYS_INLINE u32 height() const { return m_size.height(); }
    NODISCARD ALWAYS_INLINE usize pitch() const { return m_pitch; }
    NODISCARD ALWAYS_INLINE usize number_of_rows() const { return m_number_of_rows; }
    NODISCARD ALWAYS_INLINE usize bytes_per_pixel() const { return Gfx::bytes_per_pixel(m_format); }

    NODISCARD ALWAYS_INLINE IntRect rect() const { return { IntPoint::zero(), size() }; }

public:
    void set_pixel(u32 x, u32 y, Color);
    void set_pixels_in_row(u32 x, u32 y, u32 number_of_pixels, Color);

private:
    Bitmap(ByteBuffer, BitmapFormat, IntSize, usize pitch, usize number_of_rows);

private:
    ByteBuffer m_buffer;
    BitmapFormat m_format;
    IntSize m_size;
    usize m_pitch;
    usize m_number_of_rows;
};

} // namespace Gfx
