/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/CompareResult.h"
#include "AND/Forward.h"
#include "AND/Span.h"
#include "AND/Utf8CodepointIterator.h"

namespace AND {

class StringView {
public:
    NODISCARD static StringView from_buffer(ROByteSpan buffer);
    NODISCARD static StringView from_buffer(void const* data, usize size);
    NODISCARD static StringView from_unverified(ROByteSpan buffer);
    NODISCARD static StringView from_unverified(void const* data, usize size);
    NODISCARD static StringView from_null_terminated(char const* characters);

public:
    ALWAYS_INLINE constexpr StringView()
        : m_bytes(nullptr)
        , m_byte_count(0)
    {
    }

    ALWAYS_INLINE constexpr StringView(StringView const& other)
        : m_bytes(other.m_bytes)
        , m_byte_count(other.m_byte_count)
    {
    }

    ALWAYS_INLINE constexpr StringView(StringView&& other) noexcept
        : m_bytes(other.m_bytes)
        , m_byte_count(other.m_byte_count)
    {
        other.m_bytes = nullptr;
        other.m_byte_count = 0;
    }

    ALWAYS_INLINE constexpr StringView& operator=(StringView const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_bytes = other.m_bytes;
        m_byte_count = other.m_byte_count;
        return *this;
    }

    ALWAYS_INLINE constexpr StringView& operator=(StringView&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_bytes = other.m_bytes;
        m_byte_count = other.m_byte_count;
        other.m_bytes = nullptr;
        other.m_byte_count = 0;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE constexpr ROBytes bytes() const { return m_bytes; }
    NODISCARD ALWAYS_INLINE constexpr usize byte_count() const { return m_byte_count; }
    NODISCARD ALWAYS_INLINE constexpr ROByteSpan byte_span() const { return ROByteSpan(m_bytes, m_byte_count); }

    NODISCARD ALWAYS_INLINE constexpr bool is_empty() const { return (m_byte_count == 0); }
    NODISCARD ALWAYS_INLINE constexpr bool has_bytes() const { return (m_byte_count > 0); }

    NODISCARD ALWAYS_INLINE Utf8CodePointIterator begin() const { return Utf8CodePointIterator(m_bytes, m_byte_count); }
    NODISCARD ALWAYS_INLINE Utf8CodePointIterator end() const { return Utf8CodePointIterator(m_bytes + m_byte_count, 0); }

    NODISCARD ALWAYS_INLINE Utf8ReverseCodePointIterator rbegin() const { return Utf8ReverseCodePointIterator(m_bytes, m_byte_count); }
    NODISCARD ALWAYS_INLINE Utf8ReverseCodePointIterator rend() const { return Utf8ReverseCodePointIterator(m_bytes, 0); }

public:
    NODISCARD bool validate() const;
    NODISCARD usize calculate_length() const;

    NODISCARD Optional<usize> find(u32 code_point) const;
    NODISCARD Optional<usize> find(StringView substring) const;

    NODISCARD Optional<usize> find_last_of(u32 code_point) const;
    NODISCARD Optional<usize> find_last_of(StringView substring) const;

    NODISCARD bool contains(u32 code_point) const;
    NODISCARD bool contains(StringView substring) const;

    NODISCARD bool starts_with(u32 code_point) const;
    NODISCARD bool starts_with(StringView substring) const;
    NODISCARD bool ends_with(u32 code_point) const;
    NODISCARD bool ends_with(StringView substring) const;

    NODISCARD bool equals(StringView const& rhs) const;
    NODISCARD bool equals_ignoring_case(StringView const& rhs) const;

    NODISCARD CompareResult compare(StringView rhs) const;
    NODISCARD CompareResult compare_ignoring_case(StringView rhs) const;

    NODISCARD ALWAYS_INLINE bool operator==(StringView const& rhs) const { return equals(rhs); }
    NODISCARD ALWAYS_INLINE bool operator!=(StringView const& rhs) const { return !equals(rhs); }

    // Base case for the templated version.
    NODISCARD ALWAYS_INLINE static constexpr bool is_one_of()
    {
        return false;
    }

    template<typename T, typename... Args>
    NODISCARD ALWAYS_INLINE bool is_one_of(T const& value, Args&&... args) const
    {
        if (value == *this)
            return true;
        return is_one_of(forward<Args>(args)...);
    }

public:
    NODISCARD StringView slice(usize byte_offset) const;
    NODISCARD StringView slice(usize start_byte_offset, usize end_byte_offset) const;

private:
    ALWAYS_INLINE constexpr StringView(ROBytes in_bytes, usize in_byte_count)
        : m_bytes(in_bytes)
        , m_byte_count(in_byte_count)
    {
    }

private:
    ROBytes m_bytes;
    usize m_byte_count;
};

// Constructs a StringView from a string literal. Note that it doesn't verify if the provided string literal is valid UTF-8.
#define VIEW(string_literal) \
    ::AND::StringView::from_unverified(string_literal, sizeof(string_literal) - sizeof('\0'))

void append_to_builder(StringBuilder&, Optional<StringView>, StringView const&);

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::StringView;
#endif
