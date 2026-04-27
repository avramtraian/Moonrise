/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Assertions.h>
#include <AND/Types.h>

#include <new>

namespace AND {

template<NonConstNonReferenceTypename T>
class Queue {
public:
    Queue()
        : m_write_index(0)
        , m_read_index(0)
        , m_capacity(0)
        , m_elements(nullptr)
    {
    }

    ~Queue()
    {
        clear_and_shrink();
    }

    Queue(Queue const& other)
        : m_write_index(0)
        , m_read_index(0)
        , m_capacity(0)
        , m_elements(nullptr)
    {
        ensure_capacity(other.count());
        for (u64 unwrapped_index = other.m_read_index; unwrapped_index < other.m_write_index; ++unwrapped_index) {
            T const& other_element = other.m_elements[other.wrapped_index(unwrapped_index)];
            new (m_elements + write_index()) T(other_element);
            m_write_index++;
        }
    }

    Queue(Queue&& other) noexcept
        : m_write_index(other.m_write_index)
        , m_read_index(other.m_read_index)
        , m_capacity(other.m_capacity)
        , m_elements(other.m_elements)
    {
        other.m_write_index = 0;
        other.m_read_index = 0;
        other.m_capacity = 0;
        other.m_elements = nullptr;
    }

    Queue& operator=(Queue const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        clear();
        ensure_capacity(other.count());

        for (u64 unwrapped_index = other.m_read_index; unwrapped_index < other.m_write_index; ++unwrapped_index) {
            T const& other_element = other.m_elements[other.wrapped_index(unwrapped_index)];
            new (m_elements + write_index()) T(other_element);
            m_write_index++;
        }

        return *this;
    }

    Queue& operator=(Queue&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        clear_and_shrink();

        m_write_index = other.m_write_index;
        m_read_index = other.m_read_index;
        m_capacity = other.m_capacity;
        m_elements = other.m_elements;

        other.m_write_index = 0;
        other.m_read_index = 0;
        other.m_capacity = 0;
        other.m_elements = nullptr;
        return *this;
    }

public:
    usize count() const { return m_write_index - m_read_index; }
    bool is_empty() const { return (m_write_index == m_read_index); }
    bool has_elements() const { return !is_empty(); }

    void enqueue(T const& element)
    {
        ensure_capacity(count() + 1);
        new (m_elements + write_index()) T(element);
        m_write_index++;
    }

    void enqueue(T&& element)
    {
        ensure_capacity(count() + 1);
        new (m_elements + write_index()) T(move(element));
        m_write_index++;
    }

    T dequeue()
    {
        if (is_empty())
            PANIC("Trying to dequeue from an empty Queue!");

        usize index = read_index();
        T element = move(m_elements[index]);
        m_elements[index].~T();
        m_read_index++;
        return element;
    }

    T& peek()
    {
        if (is_empty())
            PANIC("Trying to peek into an empty Queue!");
        return m_elements[read_index()];
    }

    T const& peek() const
    {
        if (is_empty())
            PANIC("Trying to peek into an empty Queue!");
        return m_elements[read_index()];
    }

    void clear()
    {
        for (u64 unwrapped_index = m_read_index; unwrapped_index < m_write_index; unwrapped_index++) {
            usize index = wrapped_index(unwrapped_index);
            m_elements[index].~T();
        }
        m_write_index = 0;
        m_read_index = 0;
    }

    void clear_and_shrink()
    {
        clear();
        Queue::free_memory(m_elements, m_capacity);
        m_elements = nullptr;
        m_capacity = 0;
    }

    void shrink_to_fit()
    {
        if (m_capacity == count())
            return;
        set_capacity(count());
    }

    void ensure_capacity(usize minimum_capacity)
    {
        if (m_capacity >= minimum_capacity)
            return;
        set_capacity(minimum_capacity);
    }

public:
    template<typename Predicate>
    void for_each(Predicate predicate)
    {
        for (u64 unwrapped_index = m_read_index; unwrapped_index < m_write_index; unwrapped_index++) {
            usize index = wrapped_index(unwrapped_index);
            T& element = m_elements[index];
            predicate(element);
        }
    }

    template<typename Predicate>
    void for_each(Predicate predicate) const
    {
        for (u64 unwrapped_index = m_read_index; unwrapped_index < m_write_index; unwrapped_index++) {
            usize index = wrapped_index(unwrapped_index);
            T const& element = m_elements[index];
            predicate(element);
        }
    }

private:
    NODISCARD static T* allocate_memory(usize in_capacity)
    {
        usize size_in_bytes = in_capacity * sizeof(T);
        void* memory = ::operator new(size_in_bytes);
        ASSERT(memory);
        return static_cast<T*>(memory);
    }

    static void free_memory(T* in_elements, usize in_capacity)
    {
        MAYBE_UNUSED usize size_in_bytes = in_capacity * sizeof(T);
        ::operator delete(in_elements);
    }

    usize wrapped_index(u64 index) const { return index & (m_capacity - 1); }
    usize write_index() const { return wrapped_index(m_write_index); }
    usize read_index() const { return wrapped_index(m_read_index); }

    void set_capacity(usize required_capacity)
    {
        ASSERT(required_capacity >= count());

        // NOTE: Since the capacity of the Queue must always be a power of two, we have to ensure that the provided
        //       minimum capacity does not exceed the biggest power of two a 'usize' can store.
        static constexpr usize max_capacity = static_cast<usize>(1) << (8 * sizeof(usize) - 1);
        if (required_capacity > max_capacity)
            PANIC("The required capacity exceeds the maximum capacity of the Queue!");

        if (required_capacity == m_capacity)
            return;

        usize new_capacity = (required_capacity > 0) ? 1 : 0;
        while (new_capacity < required_capacity)
            new_capacity *= 2;

        T* new_elements = Queue::allocate_memory(new_capacity);
        usize new_write_index = 0;
        for (u64 unwrapped_index = m_read_index; unwrapped_index < m_write_index; unwrapped_index++) {
            usize index = wrapped_index(unwrapped_index);
            new (new_elements + new_write_index) T(move(m_elements[index]));
            m_elements[index].~T();
            ++new_write_index;
        }

        Queue::free_memory(m_elements, m_capacity);
        m_elements = new_elements;
        m_capacity = new_capacity;
        m_write_index = new_write_index;
        m_read_index = 0;
    }

private:
    u64 m_write_index;
    u64 m_read_index;
    usize m_capacity;
    T* m_elements;
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::Queue;
#endif
