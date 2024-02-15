/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertion.h"
#include "AT/CoreDefines.h"
#include "AT/CoreTypes.h"

namespace AT {

template<typename T>
class Span {
public:
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = T*;
    using ReverseConstIterator = const T*;

public:
    ALWAYS_INLINE constexpr Span()
        : m_elements(nullptr)
        , m_count(0)
    {}

    constexpr Span(const Span& other) = default;
    constexpr Span& operator=(const Span& other) = default;

    ALWAYS_INLINE constexpr Span(Span&& other) noexcept
        : m_elements(other.m_elements)
        , m_count(other.m_count)
    {
        other.m_elements = nullptr;
        other.m_count = 0;
    }

    ALWAYS_INLINE constexpr Span(const Span<RemoveConst<T>>& other)
    requires (is_const<T>)
        : m_elements(other.m_elements)
        , m_count(other.m_count)
    {}

    ALWAYS_INLINE constexpr Span(T* elements, usize count)
        : m_elements(elements)
        , m_count(count)
    {}

    ALWAYS_INLINE constexpr Span& operator=(Span&& other) noexcept
    {
        m_elements = other.m_elements;
        m_count = other.m_count;
        other.m_elements = nullptr;
        other.m_count = 0;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE T* elements() { return m_elements; }
    NODISCARD ALWAYS_INLINE const T* elements() const { return m_elements; }

    NODISCARD ALWAYS_INLINE T* operator*() { return elements(); }
    NODISCARD ALWAYS_INLINE const T* operator*() const { return elements(); }

    NODISCARD ALWAYS_INLINE usize count() const { return m_count; }
    NODISCARD ALWAYS_INLINE static constexpr usize element_size() { return sizeof(T); }

    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_count == 0); }
    NODISCARD ALWAYS_INLINE bool has_elements() const { return (m_count > 0); }

public:
    NODISCARD ALWAYS_INLINE T& at(usize index)
    {
        AT_ASSERT(index < m_count);
        return m_elements[index];
    }

    NODISCARD ALWAYS_INLINE const T& at(usize index) const
    {
        AT_ASSERT(index < m_count);
        return m_elements[index];
    }

    NODISCARD ALWAYS_INLINE T& operator[](usize index) { return at(index); }
    NODISCARD ALWAYS_INLINE const T& operator[](usize index) const { return at(index); }

    NODISCARD ALWAYS_INLINE T& first()
    {
        AT_ASSERT(has_elements());
        return m_elements[0];
    }

    NODISCARD ALWAYS_INLINE const T& first() const
    {
        AT_ASSERT(has_elements());
        return m_elements[0];
    }

    NODISCARD ALWAYS_INLINE T& last()
    {
        AT_ASSERT(has_elements());
        return m_elements[m_count - 1];
    }

    NODISCARD ALWAYS_INLINE const T& last() const
    {
        AT_ASSERT(has_elements());
        return m_elements[m_count - 1];
    }

public:
    NODISCARD ALWAYS_INLINE Span<T> slice(usize offset)
    {
        AT_ASSERT(offset <= m_count);
        return Span<T>(m_elements + offset, m_count - offset);
    }

    NODISCARD ALWAYS_INLINE Span<const T> slice(usize offset) const
    {
        AT_ASSERT(offset <= m_count);
        return Span<const T>(m_elements + offset, m_count - offset);
    }

    NODISCARD ALWAYS_INLINE Span<T> slice(usize offset, usize count)
    {
        AT_ASSERT(count + offset <= m_count);
        return Span<T>(m_elements + offset, count);
    }

    NODISCARD ALWAYS_INLINE Span<const T> slice(usize offset, usize count) const
    {
        AT_ASSERT(count + offset <= m_count);
        return Span<const T>(m_elements + offset, count);
    }

    template<typename Q>
    requires (Span<Q>::element_size() == element_size())
    NODISCARD ALWAYS_INLINE Span<Q> as() const
    {
        return Span<Q>(reinterpret_cast<Q*>(m_elements), m_count);
    }

public:
    NODISCARD ALWAYS_INLINE Iterator begin() { return Iterator(m_elements); }
    NODISCARD ALWAYS_INLINE Iterator end() { return Iterator(m_elements + m_count); }

    NODISCARD ALWAYS_INLINE ConstIterator begin() const { return ConstIterator(m_elements); }
    NODISCARD ALWAYS_INLINE ConstIterator end() const { return ConstIterator(m_elements + m_count); }

    NODISCARD ALWAYS_INLINE ReverseIterator rbegin() { return Iterator(m_elements + m_count - 1); }
    NODISCARD ALWAYS_INLINE ReverseIterator rend() { return Iterator(m_elements - 1); }

    NODISCARD ALWAYS_INLINE ReverseConstIterator rbegin() const { return Iterator(m_elements + m_count - 1); }
    NODISCARD ALWAYS_INLINE ReverseConstIterator rend() const { return Iterator(m_elements - 1); }

private:
    T* m_elements;
    usize m_count;
};

using ReadonlyByteSpan = Span<ReadonlyByte>;
using WriteonlyByteSpan = Span<WriteonlyByte>;
using ReadWriteByteSpan = Span<ReadWriteByte>;

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::ReadonlyByteSpan;
using AT::ReadWriteByteSpan;
using AT::Span;
using AT::WriteonlyByteSpan;
#endif // AT_INCLUDE_GLOBALLY