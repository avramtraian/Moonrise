/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

template<typename T>
requires(!is_rvalue_reference<T>)
class ArrayIterator {
public:
    using StorageType = RemoveReference<T>;

public:
    ALWAYS_INLINE explicit ArrayIterator(StorageType* current)
        : m_current(current)
    {
    }

    NODISCARD ALWAYS_INLINE StorageType& operator*() const { return *m_current; }
    NODISCARD ALWAYS_INLINE StorageType* operator->() const { return m_current; }

    ALWAYS_INLINE ArrayIterator& operator++()
    {
        ++m_current;
        return *this;
    }

    ALWAYS_INLINE ArrayIterator operator++(int)
    {
        ArrayIterator copy = *this;
        this->operator++();
        return copy;
    }

    ALWAYS_INLINE bool operator==(ArrayIterator const& other) const { return m_current == other.m_current; }
    ALWAYS_INLINE bool operator!=(ArrayIterator const& other) const { return m_current != other.m_current; }

private:
    StorageType* m_current;
};

template<typename T>
requires(!is_rvalue_reference<T>)
class ArrayReverseIterator {
public:
    using StorageType = RemoveReference<T>;

public:
    ALWAYS_INLINE explicit ArrayReverseIterator(StorageType* current)
        : m_current(current)
    {
    }

    NODISCARD ALWAYS_INLINE StorageType& operator*() const { return *m_current; }
    NODISCARD ALWAYS_INLINE StorageType* operator->() const { return m_current; }

    ALWAYS_INLINE ArrayReverseIterator& operator++()
    {
        --m_current;
        return *this;
    }

    ALWAYS_INLINE ArrayReverseIterator operator++(int)
    {
        ArrayIterator copy = *this;
        this->operator++();
        return copy;
    }

    NODISCARD ALWAYS_INLINE bool operator==(ArrayReverseIterator const& other) const { return m_current == other.m_current; }
    NODISCARD ALWAYS_INLINE bool operator!=(ArrayReverseIterator const& other) const { return m_current != other.m_current; }

private:
    StorageType* m_current;
};

} // namespace AND
