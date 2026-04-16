/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Utf8CodepointIterator.h"

namespace AND {

//=================================================================================================
// FORWARD ITERATOR.
//=================================================================================================

Utf8CodePointIterator::Utf8CodePointIterator(ROBytes bytes, usize byte_count)
    : m_bytes(bytes)
    , m_byte_count(byte_count)
    , m_current_decode_result(utf8_decode(m_bytes, m_byte_count))
{
}

u32 Utf8CodePointIterator::operator*() const
{
    ASSERTF(!is_done(), "Utf8CodePointIterator::operator*() called on invalid iterator!");
    if (m_current_decode_result.has_value())
        return m_current_decode_result.value().code_point;

    static constexpr u32 unicode_replacement_character = 0xFFFD;
    return unicode_replacement_character;
}

usize Utf8CodePointIterator::code_point_byte_width() const
{
    ASSERTF(!is_done(), "Utf8CodePointIterator::code_point_byte_width() called on invalid iterator!");
    if (m_current_decode_result.has_value())
        return m_current_decode_result.value().byte_width;
    return sizeof(ROByte);
}

Utf8CodePointIterator& Utf8CodePointIterator::operator++()
{
    ASSERTF(!is_done(), "Utf8CodePointIterator::operator++() called on invalid iterator!");

    usize code_point_byte_width = 1;
    if (m_current_decode_result.has_value())
        code_point_byte_width = m_current_decode_result.value().byte_width;

    m_bytes += code_point_byte_width;
    m_byte_count -= code_point_byte_width;
    m_current_decode_result = utf8_decode(m_bytes, m_byte_count);

    return *this;
}

Utf8CodePointIterator Utf8CodePointIterator::operator++(int)
{
    Utf8CodePointIterator copy = *this;
    this->operator++();
    return copy;
}

bool Utf8CodePointIterator::operator==(Utf8CodePointIterator const& other) const
{
    return m_bytes == other.m_bytes && m_byte_count == other.m_byte_count;
}

bool Utf8CodePointIterator::operator!=(Utf8CodePointIterator const& other) const
{
    bool are_equal = (*this == other);
    return !are_equal;
}

Optional<u32> Utf8CodePointIterator::peek(u32 offset_in_code_points) const
{
    Utf8CodePointIterator peek_iterator = *this;
    for (usize i = 0; i < offset_in_code_points; i++) {
        if (peek_iterator.is_done())
            return {};
        peek_iterator.operator++();
    }

    if (!peek_iterator.is_done())
        return *peek_iterator;
    return {};
}

bool Utf8CodePointIterator::code_point_is_valid() const
{
    return m_current_decode_result.has_value();
}

bool Utf8CodePointIterator::is_done() const
{
    return (m_byte_count == 0);
}

//=================================================================================================
// REVERSE ITERATOR.
//=================================================================================================

Utf8ReverseCodePointIterator::Utf8ReverseCodePointIterator(ROBytes bytes, usize byte_count)
    : m_bytes(bytes)
    , m_byte_count(byte_count)
    , m_current_decode_result(utf8_decode_reversed(m_bytes, m_byte_count))
{
}

u32 Utf8ReverseCodePointIterator::operator*() const
{
    ASSERTF(!is_done(), "Utf8ReverseCodePointIterator::operator*() called on invalid iterator!");
    if (m_current_decode_result.has_value())
        return m_current_decode_result.value().code_point;

    static constexpr u32 unicode_replacement_character = 0xFFFD;
    return unicode_replacement_character;
}

usize Utf8ReverseCodePointIterator::code_point_byte_width() const
{
    ASSERTF(!is_done(), "Utf8ReverseCodePointIterator::code_point_byte_width() called on invalid iterator!");
    if (m_current_decode_result.has_value())
        return m_current_decode_result.value().byte_width;
    return sizeof(ROByte);
}

Utf8ReverseCodePointIterator& Utf8ReverseCodePointIterator::operator++()
{
    ASSERTF(!is_done(), "Utf8ReverseCodePointIterator::operator++() called on invalid iterator!");

    usize code_point_byte_width = 1;
    if (m_current_decode_result.has_value())
        code_point_byte_width = m_current_decode_result.value().byte_width;

    m_byte_count -= code_point_byte_width;
    m_current_decode_result = utf8_decode_reversed(m_bytes, m_byte_count);

    return *this;
}

Utf8ReverseCodePointIterator Utf8ReverseCodePointIterator::operator++(int)
{
    Utf8ReverseCodePointIterator copy = *this;
    this->operator++();
    return copy;
}

bool Utf8ReverseCodePointIterator::operator==(Utf8ReverseCodePointIterator const& other) const
{
    return m_bytes == other.m_bytes && m_byte_count == other.m_byte_count;
}

bool Utf8ReverseCodePointIterator::operator!=(Utf8ReverseCodePointIterator const& other) const
{
    bool are_equal = (*this == other);
    return !are_equal;
}

Optional<u32> Utf8ReverseCodePointIterator::peek(u32 offset_in_code_points) const
{
    Utf8ReverseCodePointIterator peek_iterator = *this;
    for (usize i = 0; i < offset_in_code_points; i++) {
        if (peek_iterator.is_done())
            return {};
        peek_iterator.operator++();
    }

    if (!peek_iterator.is_done())
        return *peek_iterator;
    return {};
}

bool Utf8ReverseCodePointIterator::code_point_is_valid() const
{
    return m_current_decode_result.has_value();
}

bool Utf8ReverseCodePointIterator::is_done() const
{
    return (m_byte_count == 0);
}

} // namespace AND
