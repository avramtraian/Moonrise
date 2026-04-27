/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Assertions.h>
#include <AND/Types.h>
#include <AND/Vector.h>

namespace AND {

template<NonConstNonReferenceTypename T>
class Stack {
public:
    Stack() = default;
    ~Stack() = default;

    Stack(Stack const& other) = default;
    Stack(Stack&& other) noexcept = default;
    Stack& operator=(Stack const& other) = default;
    Stack& operator=(Stack&& other) noexcept = default;

public:
    bool is_empty() const { return m_elements.is_empty(); }
    bool has_elements() const { return m_elements.has_elements(); }
    usize count() const { return m_elements.count(); }

    void push(T const& element)
    {
        m_elements.push_back(element);
    }

    void push(T&& element)
    {
        m_elements.push_back(move(element));
    }

    T pop()
    {
        if (m_elements.is_empty())
            PANIC("Trying to pop from an empty Stack!");
        return m_elements.take_last();
    }

    T& peek()
    {
        if (m_elements.is_empty())
            PANIC("Trying to peek into an empty Stack!");
        return m_elements.last();
    }

    T const& peek() const
    {
        if (m_elements.is_empty())
            PANIC("Trying to peek into an empty Stack!");
        return m_elements.last();
    }

    // Operations inherited from the internal Vector container.
    void clear() { m_elements.clear(); }
    void shrink_to_fit() { m_elements.shrink_to_fit(); }
    void clear_and_shrink() { m_elements.clear_and_shrink(); }
    void ensure_capacity(usize minimum_capacity) { m_elements.ensure_capacity(minimum_capacity); }

public:
    template<typename Predicate>
    void for_each(Predicate predicate)
    {
        m_elements.for_each(predicate);
    }

    template<typename Predicate>
    void for_each(Predicate predicate) const
    {
        m_elements.for_each(predicate);
    }

private:
    Vector<T> m_elements;
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::Stack;
#endif
