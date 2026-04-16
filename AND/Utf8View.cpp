/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Utf8View.h"
#include "AND/StringBuilder.h"
#include "AND/UnicodeCodePoint.h"

namespace AND {

Utf8View Utf8View::from_buffer(ROByteSpan buffer)
{
    Utf8View view = Utf8View { buffer.bytes(), buffer.count() };
    ASSERTF(view.validate(), "Trying to construct a Utf8View from a buffer that is not correctly encoded as UTF-8!");
    return view;
}

Utf8View Utf8View::from_buffer(void const* data, usize size)
{
    ROByteSpan buffer = ROByteSpan { static_cast<ROByte const*>(data), size };
    return Utf8View::from_buffer(buffer);
}

Utf8View Utf8View::from_unverified(ROByteSpan buffer)
{
    return Utf8View { buffer.bytes(), buffer.count() };
}

Utf8View Utf8View::from_unverified(void const* data, usize size)
{
    ROByteSpan buffer = ROByteSpan { static_cast<ROByte const*>(data), size };
    return Utf8View::from_unverified(buffer);
}

Utf8View Utf8View::from_null_terminated(char const* characters)
{
    if (characters == nullptr)
        return {};

    ROBytes bytes = reinterpret_cast<ROBytes>(characters);
    usize byte_count = 0;
    while (*characters) {
        byte_count++;
        characters++;
    }

    return Utf8View::from_buffer(bytes, byte_count);
}

bool Utf8View::validate() const
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

usize Utf8View::calculate_length() const
{
    usize length = 0;
    for (MAYBE_UNUSED u32 code_point : *this)
        ++length;
    return length;
}

Optional<usize> Utf8View::find(u32 code_point) const
{
    usize byte_offset = 0;
    for (auto it = begin(); it != end(); ++it) {
        if (*it == code_point)
            return byte_offset;
        byte_offset += it.code_point_byte_width();
    }
    return {};
}

Optional<usize> Utf8View::find(Utf8View substring) const
{
    if (m_byte_count < substring.m_byte_count)
        return {};

    for (usize byte_offset = 0; byte_offset < m_byte_count; ++byte_offset) {
        Utf8View source_slice = slice(byte_offset);
        if (source_slice.starts_with(substring))
            return byte_offset;
    }

    return {};
}

Optional<usize> Utf8View::find_last_of(u32 code_point) const
{
    usize byte_offset = m_byte_count;
    for (auto it = rbegin(); it != rend(); ++it) {
        byte_offset -= it.code_point_byte_width();
        if (*it == code_point)
            return byte_offset;
    }
    return {};
}

Optional<usize> Utf8View::find_last_of(Utf8View substring) const
{
    if (m_byte_count < substring.m_byte_count)
        return {};

    for (ssize byte_offset = m_byte_count - substring.m_byte_count; byte_offset >= 0; --byte_offset) {
        Utf8View source_slice = slice(byte_offset);
        if (source_slice.starts_with(substring))
            return byte_offset;
    }

    return {};
}

bool Utf8View::contains(u32 code_point) const
{
    Optional<usize> byte_offset = find(code_point);
    return byte_offset.has_value();
}

bool Utf8View::contains(Utf8View substring) const
{
    Optional<usize> byte_offset = find(substring);
    return byte_offset.has_value();
}

bool Utf8View::starts_with(u32 code_point) const
{
    if (is_empty())
        return false;

    auto iterator = begin();
    return (*iterator == code_point);
}

bool Utf8View::starts_with(Utf8View substring) const
{
    if (m_byte_count < substring.m_byte_count)
        return false;

    auto source_slice = slice(0, substring.m_byte_count);
    return source_slice == substring;
}

bool Utf8View::ends_with(u32 code_point) const
{
    if (is_empty())
        return false;

    auto iterator = rbegin();
    return (*iterator == code_point);
}

bool Utf8View::ends_with(Utf8View substring) const
{
    if (m_byte_count < substring.m_byte_count)
        return false;

    auto source_slice = slice(m_byte_count - substring.m_byte_count);
    return source_slice == substring;
}

bool Utf8View::equals(Utf8View const& rhs) const
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

bool Utf8View::equals_ignoring_case(Utf8View const& rhs) const
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

CompareResult Utf8View::compare(Utf8View rhs) const
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

CompareResult Utf8View::compare_ignoring_case(Utf8View rhs) const
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

Utf8View Utf8View::slice(usize byte_offset) const
{
    if (byte_offset >= m_byte_count)
        return {};

    ROBytes bytes = m_bytes + byte_offset;
    usize byte_count = m_byte_count - byte_offset;
    return Utf8View { bytes, byte_count };
}

Utf8View Utf8View::slice(usize start_byte_offset, usize end_byte_offset) const
{
    ASSERTF(start_byte_offset <= end_byte_offset, "Invalid order of offsets provided to Utf8::slice()!");
    if (start_byte_offset >= m_byte_count)
        return {};

    if (end_byte_offset >= m_byte_count)
        end_byte_offset = m_byte_count;

    ROBytes bytes = m_bytes + start_byte_offset;
    usize byte_count = end_byte_offset - start_byte_offset;
    return Utf8View { bytes, byte_count };
}

void append_to_builder(StringBuilder& builder, Optional<Utf8View>, Utf8View const& view)
{
    builder.append_utf8(view);
}

} // namespace AND
