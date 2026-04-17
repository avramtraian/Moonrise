/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Span.h"
#include "AND/Utf16CodePointIterator.h"

namespace AND {

class Utf16View {
public:
    NODISCARD static Utf16View from_code_units(Span<u16 const> code_units);
    NODISCARD static Utf16View from_code_units(u16 const* code_units, usize code_unit_count);
    NODISCARD static Utf16View from_null_terminated(char16_t const* characters);

public:
    ALWAYS_INLINE constexpr Utf16View()
        : m_code_units(nullptr)
        , m_code_unit_count(0)
    {
    }

    ALWAYS_INLINE constexpr Utf16View(Utf16View const& other)
        : m_code_units(other.m_code_units)
        , m_code_unit_count(other.m_code_unit_count)
    {
    }

    ALWAYS_INLINE constexpr Utf16View(Utf16View&& other) noexcept
        : m_code_units(other.m_code_units)
        , m_code_unit_count(other.m_code_unit_count)
    {
        other.m_code_units = nullptr;
        other.m_code_unit_count = 0;
    }

    ALWAYS_INLINE constexpr Utf16View& operator=(Utf16View const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_code_units = other.m_code_units;
        m_code_unit_count = other.m_code_unit_count;

        return *this;
    }

    ALWAYS_INLINE constexpr Utf16View& operator=(Utf16View&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_code_units = other.m_code_units;
        m_code_unit_count = other.m_code_unit_count;
        other.m_code_units = nullptr;
        other.m_code_unit_count = 0;

        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE constexpr u16 const* code_units() const { return m_code_units; }
    NODISCARD ALWAYS_INLINE constexpr usize code_unit_count() const { return m_code_unit_count; }

    NODISCARD ALWAYS_INLINE constexpr bool is_empty() const { return (m_code_unit_count == 0); }
    NODISCARD ALWAYS_INLINE constexpr bool has_code_units() const { return (m_code_unit_count > 0); }

    NODISCARD ALWAYS_INLINE ROBytes bytes() const { return reinterpret_cast<ROBytes>(m_code_units); }
    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_code_unit_count * sizeof(u16); }
    NODISCARD ALWAYS_INLINE ROByteSpan byte_span() const { return ROByteSpan { bytes(), byte_count() }; }

    NODISCARD ALWAYS_INLINE Utf16CodePointIterator begin() const { return Utf16CodePointIterator(m_code_units, m_code_unit_count); }
    NODISCARD ALWAYS_INLINE Utf16CodePointIterator end() const { return Utf16CodePointIterator(m_code_units + m_code_unit_count, 0); }

    NODISCARD ALWAYS_INLINE Utf16ReverseCodePointIterator rbegin() const { return Utf16ReverseCodePointIterator(m_code_units, m_code_unit_count); }
    NODISCARD ALWAYS_INLINE Utf16ReverseCodePointIterator rend() const { return Utf16ReverseCodePointIterator(m_code_units, 0); }

public:
    NODISCARD bool validate() const;
    NODISCARD usize calculate_length() const;
    void clear();

private:
    ALWAYS_INLINE constexpr Utf16View(u16 const* code_units, usize code_unit_count)
        : m_code_units(code_units)
        , m_code_unit_count(code_unit_count)
    {
    }

private:
    u16 const* m_code_units;
    usize m_code_unit_count;
};

#define UTF16_VIEW(string_literal)                              \
    ::AND::Utf16View::from_code_units(                          \
        reinterpret_cast<::AND::u16 const*>(u##string_literal), \
        (sizeof(u##string_literal) - sizeof(u'\0')) / sizeof(::AND::u16))

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::Utf16View;
#endif
