/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/ArrayIterator.h"
#include "AND/Assertions.h"
#include "AND/Types.h"

namespace AND {

template<typename T>
requires(!is_reference<T>)
class Span {
    template<typename Q>
    requires(!is_reference<Q>)
    friend class Span;

    using Iterator = ArrayIterator<T>;
    using ReverseIterator = ArrayReverseIterator<T>;

    static constexpr bool is_byte_span = is_same<T, ROByte> || is_same<T, WOByte> || is_same<T, RWByte>;

public:
    ALWAYS_INLINE constexpr Span()
        : m_elements(nullptr)
        , m_count(0)
    {
    }

    ALWAYS_INLINE constexpr Span(T* elements, usize count)
        : m_elements(elements)
        , m_count(count)
    {
    }

    ALWAYS_INLINE constexpr Span(Span const& other)
        : m_elements(other.m_elements)
        , m_count(other.m_count)
    {
    }

    /*implicit*/ ALWAYS_INLINE constexpr Span(Span<RemoveConst<T>> const& other)
    requires(is_const<T>)
        : m_elements(other.m_storage)
        , m_count(other.m_count)
    {
    }

    ALWAYS_INLINE constexpr Span(Span&& other) noexcept
        : m_elements(other.m_elements)
        , m_count(other.m_count)
    {
        other.m_elements = nullptr;
        other.m_count = 0;
    }

    /*implicit*/ ALWAYS_INLINE constexpr Span(Span<RemoveConst<T>>&& other) noexcept
    requires(is_const<T>)
        : m_elements(other.m_storage)
        , m_count(other.m_count)
    {
        other.m_storage = nullptr;
        other.m_count = 0;
    }

    ALWAYS_INLINE constexpr Span& operator=(Span const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_elements = other.m_elements;
        m_count = other.m_count;
        return *this;
    }

    ALWAYS_INLINE constexpr Span& operator=(Span<RemoveConst<T>> const& other)
    requires(is_const<T>)
    {
        m_elements = other.m_storage;
        m_count = other.m_count;
        return *this;
    }

    ALWAYS_INLINE constexpr Span& operator=(Span&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        m_elements = other.m_elements;
        m_count = other.m_count;
        other.m_elements = nullptr;
        other.m_count = 0;
        return *this;
    }

    ALWAYS_INLINE constexpr Span& operator=(Span<RemoveConst<T>>&& other) noexcept
    requires(is_const<T>)
    {
        m_elements = other.m_storage;
        m_count = other.m_count;
        other.m_storage = nullptr;
        other.m_count = 0;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE constexpr T* elements() const { return m_elements; }
    NODISCARD ALWAYS_INLINE constexpr usize count() const { return m_count; }

    NODISCARD ALWAYS_INLINE constexpr bool is_empty() const { return (m_count == 0); }
    NODISCARD ALWAYS_INLINE constexpr bool has_elements() const { return (m_count > 0); }

    NODISCARD ALWAYS_INLINE constexpr const T* ro_elements() const { return m_elements; }
    NODISCARD ALWAYS_INLINE constexpr T* wo_elements() const { return m_elements; }
    NODISCARD ALWAYS_INLINE constexpr T* rw_elements() const { return m_elements; }

public:
    // Utility constructors in the context of byte spans.
    ALWAYS_INLINE constexpr Span(void* data, usize size)
    requires is_byte_span
        : m_elements(static_cast<T*>(data))
        , m_count(size)
    {
    }

    // Utility constructors in the context of byte spans.
    ALWAYS_INLINE constexpr Span(void const* data, usize size)
    requires(is_byte_span && is_const<T>)
        : m_elements(static_cast<T*>(data))
        , m_count(size)
    {
    }

    // Add a wrapper around 'elements()' with a more descriptive name in the context of byte spans.
    NODISCARD ALWAYS_INLINE constexpr T* bytes() const
    requires is_byte_span
    {
        return elements();
    }

public:
    NODISCARD ALWAYS_INLINE T& operator[](usize index) const
    {
        ASSERTF(index < m_count, "Index out-of-range in Span::operator[]()!");
        return m_elements[index];
    }

    NODISCARD ALWAYS_INLINE T& first() const
    {
        ASSERTF(has_elements(), "Trying to access the first element of an empty Span (in Span::first())!");
        return m_elements[0];
    }

    NODISCARD ALWAYS_INLINE T& last() const
    {
        ASSERTF(has_elements(), "Trying to access the last element of an empty Span (in Span::last())!");
        return m_elements[m_count - 1];
    }

public:
    NODISCARD ALWAYS_INLINE Iterator begin() const { return Iterator(m_elements); }
    NODISCARD ALWAYS_INLINE Iterator end() const { return Iterator(m_elements + m_count); }
    NODISCARD ALWAYS_INLINE ReverseIterator rbegin() const { return ReverseIterator(m_elements + m_count - 1); }
    NODISCARD ALWAYS_INLINE ReverseIterator rend() const { return ReverseIterator(m_elements - 1); }

private:
    T* m_elements;
    usize m_count;
};

template<typename T>
using ConstSpan = Span<T const>;

using ROByteSpan = Span<ROByte>;
using WOByteSpan = Span<WOByte>;
using RWByteSpan = Span<RWByte>;

} // namespace AND
