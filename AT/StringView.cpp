/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AT/StringView.h>
#include <AT/Utf8.h>

namespace AT {

StringView StringView::create_from_utf8(const char* characters, usize byte_count)
{
    MAYBE_UNUSED bool validity = UTF8::check_validity({ reinterpret_cast<ReadonlyBytes>(characters), byte_count });
    AT_ASSERT(validity);

    return unsafe_create_from_utf8(characters, byte_count);
}

StringView StringView::create_from_utf8(const char* null_terminated_characters)
{
    usize byte_count = UTF8::byte_count(reinterpret_cast<ReadonlyBytes>(null_terminated_characters));
    AT_ASSERT(byte_count != invalid_size);

    return unsafe_create_from_utf8(null_terminated_characters, byte_count);
}

usize StringView::find(char ascii_character) const
{
    for (usize offset = 0; offset < m_byte_count; ++offset) {
        if (m_characters[offset] == ascii_character) {
            return offset;
        }
    }

    return invalid_position;
}

usize StringView::find(UnicodeCodepoint codepoint) const
{
    usize offset = 0;
    while (offset < m_byte_count) {
        usize current_codepoint_width;
        UnicodeCodepoint current_codepoint = UTF8::bytes_to_codepoint(byte_span().slice(offset), current_codepoint_width);
        AT_ASSERT(current_codepoint != invalid_unicode_codepoint);

        if (current_codepoint == codepoint) {
            return offset;
        }
        offset += current_codepoint_width;
    }

    return invalid_position;
}

StringView StringView::slice(usize offset_in_bytes) const
{
    AT_ASSERT(offset_in_bytes <= m_byte_count);
    return StringView::unsafe_create_from_utf8(m_characters + offset_in_bytes, m_byte_count - offset_in_bytes);
}

StringView StringView::slice(usize offset_in_bytes, usize bytes_count) const
{
    AT_ASSERT(offset_in_bytes + bytes_count <= m_byte_count);
    return StringView::unsafe_create_from_utf8(m_characters + offset_in_bytes, bytes_count);
}

bool StringView::operator==(const StringView& other) const
{
    if (m_byte_count != other.m_byte_count) {
        return false;
    }

    for (usize byte_offset = 0; byte_offset < m_byte_count; ++byte_offset) {
        if (m_characters[byte_offset] != other.m_characters[byte_offset]) {
            return false;
        }
    }

    return true;
}

bool StringView::operator!=(const StringView& other) const
{
    const bool are_equal = ((*this) == other);
    return !are_equal;
}

} // namespace AT
