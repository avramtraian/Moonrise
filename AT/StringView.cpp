/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/StringView.h"
#include "AT/Utf8.h"

namespace AT {

ErrorOr<StringView> StringView::create_from_utf8(const char* characters, usize byte_count)
{
    TRY(UTF8::try_check_validity({ reinterpret_cast<ReadonlyBytes>(characters), byte_count }));
    return unsafe_create_from_utf8(characters, byte_count);
}

ErrorOr<StringView> StringView::create_from_utf8(const char* null_terminated_characters)
{
    TRY_ASSIGN(auto byte_count, UTF8::try_byte_count(reinterpret_cast<ReadonlyBytes>(null_terminated_characters)));
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

usize StringView::find(UnicodeCodepoint codepoint_to_find) const
{
    usize offset = 0;
    while (offset < m_byte_count) {
        usize codepoint_width;
        const auto codepoint = UTF8::bytes_to_codepoint(byte_span().slice(offset), codepoint_width);
        AT_ASSERT(codepoint != invalid_unicode_codepoint);

        if (codepoint == codepoint_to_find) {
            return offset;
        }
        offset += codepoint_width;
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