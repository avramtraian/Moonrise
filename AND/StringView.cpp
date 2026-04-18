/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/StringView.h"
#include "AND/StringBuilder.h"
#include "AND/UnicodeCodePoint.h"

namespace AND {

StringView StringView::from_buffer(ROByteSpan buffer)
{
    StringView view = StringView { buffer.bytes(), buffer.count() };
    ASSERTF(view.validate(), "Trying to construct a StringView from a buffer that is not correctly encoded as UTF-8!");
    return view;
}

StringView StringView::from_buffer(void const* data, usize size)
{
    ROByteSpan buffer = ROByteSpan { static_cast<ROByte const*>(data), size };
    return StringView::from_buffer(buffer);
}

StringView StringView::from_unverified(ROByteSpan buffer)
{
    return StringView { buffer.bytes(), buffer.count() };
}

StringView StringView::from_unverified(void const* data, usize size)
{
    ROByteSpan buffer = ROByteSpan { static_cast<ROByte const*>(data), size };
    return StringView::from_unverified(buffer);
}

StringView StringView::from_null_terminated(char const* characters)
{
    if (characters == nullptr)
        return {};

    ROBytes bytes = reinterpret_cast<ROBytes>(characters);
    usize byte_count = 0;
    while (*characters) {
        byte_count++;
        characters++;
    }

    return StringView::from_buffer(bytes, byte_count);
}

bool StringView::validate() const
{
    ROBytes bytes = m_bytes;
    usize byte_count = m_byte_count;

    while (byte_count > 0) {
        auto decode = utf8_decode(bytes, byte_count);
        if (!decode.has_value())
            return false;
        bytes += decode.value().byte_width;
        byte_count -= decode.value().byte_width;
    }

    return true;
}

usize StringView::calculate_length() const
{
    usize length = 0;
    for (MAYBE_UNUSED u32 code_point : *this)
        ++length;
    return length;
}

Optional<usize> StringView::find(u32 code_point) const
{
    usize byte_offset = 0;
    for (auto it = begin(); it != end(); ++it) {
        if (*it == code_point)
            return byte_offset;
        byte_offset += it.code_point_byte_width();
    }
    return {};
}

Optional<usize> StringView::find(StringView substring) const
{
    if (m_byte_count < substring.m_byte_count)
        return {};

    for (usize byte_offset = 0; byte_offset < m_byte_count; ++byte_offset) {
        StringView source_slice = slice(byte_offset);
        if (source_slice.starts_with(substring))
            return byte_offset;
    }

    return {};
}

Optional<usize> StringView::find_last_of(u32 code_point) const
{
    usize byte_offset = m_byte_count;
    for (auto it = rbegin(); it != rend(); ++it) {
        byte_offset -= it.code_point_byte_width();
        if (*it == code_point)
            return byte_offset;
    }
    return {};
}

Optional<usize> StringView::find_last_of(StringView substring) const
{
    if (m_byte_count < substring.m_byte_count)
        return {};

    for (ssize byte_offset = m_byte_count - substring.m_byte_count; byte_offset >= 0; --byte_offset) {
        StringView source_slice = slice(byte_offset);
        if (source_slice.starts_with(substring))
            return byte_offset;
    }

    return {};
}

bool StringView::contains(u32 code_point) const
{
    Optional<usize> byte_offset = find(code_point);
    return byte_offset.has_value();
}

bool StringView::contains(StringView substring) const
{
    Optional<usize> byte_offset = find(substring);
    return byte_offset.has_value();
}

bool StringView::starts_with(u32 code_point) const
{
    if (is_empty())
        return false;

    auto iterator = begin();
    return (*iterator == code_point);
}

bool StringView::starts_with(StringView substring) const
{
    if (m_byte_count < substring.m_byte_count)
        return false;

    auto source_slice = slice(0, substring.m_byte_count);
    return source_slice == substring;
}

bool StringView::ends_with(u32 code_point) const
{
    if (is_empty())
        return false;

    auto iterator = rbegin();
    return (*iterator == code_point);
}

bool StringView::ends_with(StringView substring) const
{
    if (m_byte_count < substring.m_byte_count)
        return false;

    auto source_slice = slice(m_byte_count - substring.m_byte_count);
    return source_slice == substring;
}

bool StringView::equals(StringView const& rhs) const
{
    auto lhs_iterator = begin();
    auto rhs_iterator = rhs.begin();

    while (!lhs_iterator.is_done() && !rhs_iterator.is_done()) {
        CompareResult result = compare_code_points(*lhs_iterator, *rhs_iterator);
        if (result.is_not_equal())
            return false;

        ++lhs_iterator;
        ++rhs_iterator;
    }

    // Both iterators should be done if the strings are equals.
    return lhs_iterator.is_done() && rhs_iterator.is_done();
}

bool StringView::equals_ignoring_case(StringView const& rhs) const
{
    auto lhs_iterator = begin();
    auto rhs_iterator = rhs.begin();

    while (!lhs_iterator.is_done() && !rhs_iterator.is_done()) {
        CompareResult result = compare_code_points_ignoring_case(*lhs_iterator, *rhs_iterator);
        if (result.is_not_equal())
            return false;

        ++lhs_iterator;
        ++rhs_iterator;
    }

    // Both iterators should be done if the strings are equals.
    return lhs_iterator.is_done() && rhs_iterator.is_done();
}

CompareResult StringView::compare(StringView rhs) const
{
    auto lhs_iterator = begin();
    auto rhs_iterator = rhs.begin();

    while (!lhs_iterator.is_done() && !rhs_iterator.is_done()) {
        CompareResult result = compare_code_points(*lhs_iterator, *rhs_iterator);
        if (result.is_not_equal())
            return result;

        ++lhs_iterator;
        ++rhs_iterator;
    }

    if (lhs_iterator.is_done())
        return CompareResult::less();
    return CompareResult::greater();
}

CompareResult StringView::compare_ignoring_case(StringView rhs) const
{
    auto lhs_iterator = begin();
    auto rhs_iterator = rhs.begin();

    while (!lhs_iterator.is_done() && !rhs_iterator.is_done()) {
        CompareResult result = compare_code_points_ignoring_case(*lhs_iterator, *rhs_iterator);
        if (result.is_not_equal())
            return result;

        ++lhs_iterator;
        ++rhs_iterator;
    }

    if (lhs_iterator.is_done())
        return CompareResult::less();
    return CompareResult::greater();
}

StringView StringView::slice(usize byte_offset) const
{
    if (byte_offset >= m_byte_count)
        return {};

    ROBytes bytes = m_bytes + byte_offset;
    usize byte_count = m_byte_count - byte_offset;
    return StringView { bytes, byte_count };
}

StringView StringView::slice(usize start_byte_offset, usize end_byte_offset) const
{
    ASSERTF(start_byte_offset <= end_byte_offset, "Invalid order of offsets provided to Utf8::slice()!");
    if (start_byte_offset >= m_byte_count)
        return {};

    if (end_byte_offset >= m_byte_count)
        end_byte_offset = m_byte_count;

    ROBytes bytes = m_bytes + start_byte_offset;
    usize byte_count = end_byte_offset - start_byte_offset;
    return StringView { bytes, byte_count };
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, StringView const& view)
{
    builder.append_utf8(view);
}

} // namespace AND
