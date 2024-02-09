/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/Utf8.h"

namespace AT {

UnicodeCodepoint UTF8::bytes_to_codepoint(ReadonlyBytes bytes, usize byte_count, usize& out_codepoint_width)
{
    if (byte_count == 0) {
        out_codepoint_width = 0;
        return invalid_unicode_codepoint;
    }

    if ((bytes[0] & 0x80) == 0x00) {
        out_codepoint_width = 1;
        return (UnicodeCodepoint)bytes[0];
    }

    if ((bytes[0] & 0xE0) == 0xC0) {
        if (byte_count < 2) {
            out_codepoint_width = 0;
            return invalid_unicode_codepoint;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (bytes[0] & 0x1F) << 6;
        codepoint += (bytes[1] & 0x3F) << 0;

        out_codepoint_width = 2;
        return codepoint;
    }

    if ((bytes[0] & 0xF0) == 0xE0) {
        if (byte_count < 3) {
            out_codepoint_width = 0;
            return invalid_unicode_codepoint;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (bytes[0] & 0x1F) << 12;
        codepoint += (bytes[1] & 0x3F) << 6;
        codepoint += (bytes[2] & 0x3F) << 0;

        out_codepoint_width = 3;
        return codepoint;
    }

    if ((bytes[0] & 0xF8) == 0xF0) {
        if (byte_count < 4) {
            out_codepoint_width = 0;
            return invalid_unicode_codepoint;
        }

        UnicodeCodepoint codepoint = 0;
        codepoint += (bytes[0] & 0x1F) << 18;
        codepoint += (bytes[1] & 0x3F) << 12;
        codepoint += (bytes[2] & 0x3F) << 6;
        codepoint += (bytes[3] & 0x3F) << 0;

        out_codepoint_width = 4;
        return codepoint;
    }

    out_codepoint_width = 0;
    return invalid_unicode_codepoint;
}

usize UTF8::bytes_to_codepoint_width(ReadonlyBytes bytes, usize byte_count)
{
    if (byte_count == 0) {
        return 0;
    }

    if ((bytes[0] & 0x80) == 0x00) {
        return 1;
    }

    if ((bytes[0] & 0xE0) == 0xC0) {
        if (byte_count < 2) {
            return 0;
        }
        return 2;
    }

    if ((bytes[0] & 0xF0) == 0xE0) {
        if (byte_count < 3) {
            return 0;
        }
        return 3;
    }

    if ((bytes[0] & 0xF8) == 0xF0) {
        if (byte_count < 4) {
            return 0;
        }
        return 4;
    }

    return 0;
}

usize UTF8::bytes_from_codepoint(
    UnicodeCodepoint codepoint, WriteonlyBytes destination_buffer, usize destination_buffer_byte_count
)
{
    if (0x0000 <= codepoint && codepoint <= 0x007F) {
        if (destination_buffer_byte_count < 1) {
            return 0;
        }
        destination_buffer[0] = (u8)codepoint;
        return 1;
    }

    if (0x0080 <= codepoint && codepoint <= 0x07FF) {
        if (destination_buffer_byte_count < 2) {
            return 0;
        }
        destination_buffer[0] = ((codepoint >> 6) & 0x1F) | 0xC0;
        destination_buffer[1] = ((codepoint >> 0) & 0x3F) | 0x80;
        return 2;
    }

    if (0x0800 <= codepoint && codepoint <= 0xFFFF) {
        if (destination_buffer_byte_count < 3) {
            return 0;
        }
        destination_buffer[0] = ((codepoint >> 12) & 0x0F) | 0xE0;
        destination_buffer[1] = ((codepoint >> 6) & 0x3F) | 0x80;
        destination_buffer[2] = ((codepoint >> 0) & 0x3F) | 0x80;
        return 3;
    }

    if (0x10000 <= codepoint) {
        if (destination_buffer_byte_count < 4) {
            return 0;
        }
        destination_buffer[0] = ((codepoint >> 18) & 0x07) | 0xF0;
        destination_buffer[1] = ((codepoint >> 12) & 0x3F) | 0x80;
        destination_buffer[2] = ((codepoint >> 6) & 0x3F) | 0x80;
        destination_buffer[3] = ((codepoint >> 0) & 0x3F) | 0x80;
        return 4;
    }

    return 0;
}

usize UTF8::codepoint_width(UnicodeCodepoint codepoint)
{
    if (0x0000 <= codepoint && codepoint <= 0x007F) {
        return 1;
    }
    if (0x0080 <= codepoint && codepoint <= 0x07FF) {
        return 2;
    }
    if (0x0800 <= codepoint && codepoint <= 0xFFFF) {
        return 3;
    }
    if (0x10000 <= codepoint) {
        return 4;
    }

    return 0;
}

usize UTF8::length(ReadonlyBytes bytes, usize byte_count)
{
    usize len = 0;
    while (byte_count) {
        usize codepoint_width = bytes_to_codepoint_width(bytes, byte_count);
        if (codepoint_width == 0) {
            return invalid_size;
        }

        ++len;
        bytes += codepoint_width;
        byte_count -= codepoint_width;
    }

    return len;
}

usize UTF8::byte_count(ReadonlyBytes bytes)
{
    if (!bytes) {
        // If the 'bytes' pointer is null, return zero - the string is empty.
        return 0;
    }

    const u8* base = bytes;
    while (*bytes++) {};

    // Determining the number of bytes this way doesn't guarantee that the byte sequence
    // is valid UTF-8, so a validation must now be performed.
    const usize byte_count = bytes - base;

    // Check that the byte sequence is actually valid UTF-8.
    usize byte_offset = 0;
    while (byte_offset < byte_count) {
        const usize codepoint_width = bytes_to_codepoint_width(base + byte_offset, byte_count - byte_offset);
        if (codepoint_width == 0) {
            // The byte sequence is not valid UTF-8.
            return invalid_size;
        }
        byte_offset += codepoint_width;
    }

    return byte_count;
}

} // namespace AT