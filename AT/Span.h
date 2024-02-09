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

    Span(const Span& other) = default;
    Span& operator=(const Span& other) = default;

    ALWAYS_INLINE constexpr Span(Span&& other) noexcept
        : m_elements(other.m_elements)
        , m_count(other.m_count)
    {
        other.m_elements = nullptr;
        other.m_count = 0;
    }

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
    NODISCARD ALWAYS_INLINE Span<T> subrange(usize offset)
    {
        AT_ASSERT(offset <= m_count);
        return Span(m_elements + offset, m_count - offset);
    }

    NODISCARD ALWAYS_INLINE Span<T> subrange(usize offset) const
    {
        AT_ASSERT(offset <= m_count);
        return Span(m_elements + offset, m_count - offset);
    }

    NODISCARD ALWAYS_INLINE Span<T> subrange(usize offset, usize count)
    {
        AT_ASSERT(count + offset <= m_count);
        return Span(m_elements + offset, count);
    }

    NODISCARD ALWAYS_INLINE Span<T> subrange(usize offset, usize count) const
    {
        AT_ASSERT(count + offset <= m_count);
        return Span(m_elements + offset, count);
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

} // namespace AT