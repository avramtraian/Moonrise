/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Span.h"

namespace AT {

template<typename T, usize inline_count>
requires (inline_count > 0)
class Array {
public:
    using Iterator = T*;
    using ConstIterator = T*;

public:
    NODISCARD ALWAYS_INLINE T* elements() { return m_elements; }
    NODISCARD ALWAYS_INLINE const T* elements() const { return m_elements; }

    NODISCARD ALWAYS_INLINE T* operator*() { return elements(); }
    NODISCARD ALWAYS_INLINE const T* operator*() const { return elements(); }

    NODISCARD ALWAYS_INLINE Span<T> span() { return Span<T>(m_elements, inline_count); }
    NODISCARD ALWAYS_INLINE Span<const T> span() const { return Span<const T>(m_elements, inline_count); }

    NODISCARD ALWAYS_INLINE static constexpr usize count() { return inline_count; }
    NODISCARD ALWAYS_INLINE static constexpr usize element_size() { return sizeof(T); }
    NODISCARD ALWAYS_INLINE static constexpr usize byte_count() { return count() * element_size(); }

    NODISCARD ALWAYS_INLINE ReadWriteByteSpan byte_span()
    {
        return ReadWriteByteSpan(reinterpret_cast<ReadWriteBytes>(m_elements), byte_count());
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span() const
    {
        return ReadonlyByteSpan(reinterpret_cast<ReadonlyBytes>(m_elements), byte_count());
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_byte_span() const { return byte_span(); }

public:
    NODISCARD ALWAYS_INLINE T& at(usize index)
    {
        AT_ASSERT(index < count());
        return m_elements[index];
    }

    NODISCARD ALWAYS_INLINE const T& at(usize index) const
    {
        AT_ASSERT(index < count());
        return m_elements[index];
    }

    NODISCARD ALWAYS_INLINE T& operator[](usize index) { return at(index); }
    NODISCARD ALWAYS_INLINE const T& operator[](usize index) const { return at(index); }

    NODISCARD ALWAYS_INLINE T& first(usize index) { return m_elements[0]; }
    NODISCARD ALWAYS_INLINE const T& first(usize index) const { return m_elements[0]; }

    NODISCARD ALWAYS_INLINE T& last(usize index) { return m_elements[inline_count - 1]; }
    NODISCARD ALWAYS_INLINE const T& last(usize index) const { return m_elements[inline_count - 1]; }

public:
    NODISCARD ALWAYS_INLINE Iterator begin() { return Iterator(m_elements); }
    NODISCARD ALWAYS_INLINE Iterator end() { return Iterator(m_elements + inline_count); }

    NODISCARD ALWAYS_INLINE ConstIterator begin() const { return ConstIterator(m_elements); }
    NODISCARD ALWAYS_INLINE ConstIterator end() const { return ConstIterator(m_elements + inline_count); }

    NODISCARD ALWAYS_INLINE Iterator rbegin() { return Iterator(m_elements + inline_count - 1); }
    NODISCARD ALWAYS_INLINE Iterator rend() { return Iterator(m_elements - 1); }

    NODISCARD ALWAYS_INLINE ConstIterator rbegin() const { return ConstIterator(m_elements + inline_count - 1); }
    NODISCARD ALWAYS_INLINE ConstIterator rend() const { return ConstIterator(m_elements - 1); }

private:
    T m_elements[inline_count];
};

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::Array;
#endif // AT_INCLUDE_GLOBALLY