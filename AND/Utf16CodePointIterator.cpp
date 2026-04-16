/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Utf16CodePointIterator.h"

namespace AND {

//=================================================================================================
// FORWARD ITERATOR.
//=================================================================================================

Utf16CodePointIterator::Utf16CodePointIterator(u16 const* code_units, usize code_unit_count)
    : m_code_units(code_units)
    , m_code_unit_count(code_unit_count)
    , m_current_decode_result(utf16_decode(m_code_units, m_code_unit_count))
{
}

u32 Utf16CodePointIterator::operator*() const
{
    ASSERTF(!is_done(), "Utf16CodePointIterator::operator*() called on invalid iterator!");
    if (m_current_decode_result.has_value())
        return m_current_decode_result.value().code_point;

    static constexpr u32 unicode_replacement_character = 0xFFFD;
    return unicode_replacement_character;
}

usize Utf16CodePointIterator::code_point_code_unit_count() const
{
    ASSERTF(!is_done(), "Utf8CodePointIterator::code_point_byte_width() called on invalid iterator!");
    if (m_current_decode_result.has_value())
        return m_current_decode_result.value().code_unit_count;
    return 1;
}

Utf16CodePointIterator& Utf16CodePointIterator::operator++()
{
    ASSERTF(!is_done(), "Utf16CodePointIterator::operator++() called on invalid iterator!");

    usize code_unit_count = 1;
    if (m_current_decode_result.has_value())
        code_unit_count = m_current_decode_result.value().code_unit_count;

    m_code_units += code_unit_count;
    m_code_unit_count -= code_unit_count;
    m_current_decode_result = utf16_decode(m_code_units, m_code_unit_count);

    return *this;
}

Utf16CodePointIterator Utf16CodePointIterator::operator++(int)
{
    Utf16CodePointIterator copy = *this;
    this->operator++();
    return copy;
}

bool Utf16CodePointIterator::operator==(Utf16CodePointIterator const& other) const
{
    return m_code_units == other.m_code_units && m_code_unit_count == other.m_code_unit_count;
}

bool Utf16CodePointIterator::operator!=(Utf16CodePointIterator const& other) const
{
    bool are_equal = (*this == other);
    return !are_equal;
}

Optional<u32> Utf16CodePointIterator::peek(u32 offset_in_code_points) const
{
    Utf16CodePointIterator peek_iterator = *this;
    for (usize i = 0; i < offset_in_code_points; i++) {
        if (peek_iterator.is_done())
            return {};
        peek_iterator.operator++();
    }

    if (!peek_iterator.is_done())
        return *peek_iterator;
    return {};
}

bool Utf16CodePointIterator::code_point_is_valid() const
{
    return m_current_decode_result.has_value();
}

bool Utf16CodePointIterator::is_done() const
{
    return (m_code_unit_count == 0);
}

//=================================================================================================
// REVERSE ITERATOR.
//=================================================================================================

Utf16ReverseCodePointIterator::Utf16ReverseCodePointIterator(u16 const* code_units, usize code_unit_count)
    : m_code_units(code_units)
    , m_code_unit_count(code_unit_count)
    , m_current_decode_result(utf16_decode_reversed(m_code_units, m_code_unit_count))
{
}

u32 Utf16ReverseCodePointIterator::operator*() const
{
    ASSERTF(!is_done(), "Utf16ReverseCodePointIterator::operator*() called on invalid iterator!");
    if (m_current_decode_result.has_value())
        return m_current_decode_result.value().code_point;

    static constexpr u32 unicode_replacement_character = 0xFFFD;
    return unicode_replacement_character;
}

usize Utf16ReverseCodePointIterator::code_point_code_unit_count() const
{
    ASSERTF(!is_done(), "Utf8CodePointIterator::code_point_byte_width() called on invalid iterator!");
    if (m_current_decode_result.has_value())
        return m_current_decode_result.value().code_unit_count;
    return 1;
}

Utf16ReverseCodePointIterator& Utf16ReverseCodePointIterator::operator++()
{
    ASSERTF(!is_done(), "Utf16ReverseCodePointIterator::operator++() called on invalid iterator!");

    usize code_unit_count = 1;
    if (m_current_decode_result.has_value())
        code_unit_count = m_current_decode_result.value().code_unit_count;

    m_code_unit_count -= code_unit_count;
    m_current_decode_result = utf16_decode_reversed(m_code_units, m_code_unit_count);

    return *this;
}

Utf16ReverseCodePointIterator Utf16ReverseCodePointIterator::operator++(int)
{
    Utf16ReverseCodePointIterator copy = *this;
    this->operator++();
    return copy;
}

bool Utf16ReverseCodePointIterator::operator==(Utf16ReverseCodePointIterator const& other) const
{
    return m_code_units == other.m_code_units && m_code_unit_count == other.m_code_unit_count;
}

bool Utf16ReverseCodePointIterator::operator!=(Utf16ReverseCodePointIterator const& other) const
{
    bool are_equal = (*this == other);
    return !are_equal;
}

Optional<u32> Utf16ReverseCodePointIterator::peek(u32 offset_in_code_points) const
{
    Utf16ReverseCodePointIterator peek_iterator = *this;
    for (usize i = 0; i < offset_in_code_points; i++) {
        if (peek_iterator.is_done())
            return {};
        peek_iterator.operator++();
    }

    if (!peek_iterator.is_done())
        return *peek_iterator;
    return {};
}

bool Utf16ReverseCodePointIterator::code_point_is_valid() const
{
    return m_current_decode_result.has_value();
}

bool Utf16ReverseCodePointIterator::is_done() const
{
    return (m_code_unit_count == 0);
}

} // namespace AND
