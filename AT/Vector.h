/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertion.h"
#include "AT/Defines.h"
#include "AT/Error.h"
#include "AT/Span.h"
#include "AT/Types.h"

namespace AT {

//
// Dynamic collection of elements that are stored contiguously in memory.
// The type of elements stored in this container must provide the ability
// to be moved in memory, as this operation is performed every time the
// vector grows, shrinks or the elements are shifted.
//
template<typename T>
class Vector {
public:
    using Iterator = T*;
    using ConstIterator = const T*;
    using ReverseIterator = T*;
    using ReverseConstIterator = const T*;

public:
    ALWAYS_INLINE static ErrorOr<Vector> create(const Vector& other)
    {
        TRY_ASSIGN(auto elements, allocate_memory(other.m_count));
        copy_elements(elements, other.m_elements, other.m_count);

        Vector vector;
        vector.m_elements = elements;
        vector.m_capacity = other.m_count;
        vector.m_count = other.m_count;
        return vector;
    }

    ALWAYS_INLINE static ErrorOr<Vector> create_with_initial_capacity(usize initial_capacity)
    {
        TRY_ASSIGN(auto elements, allocate_memory(initial_capacity));

        Vector vector;
        vector.m_elements = elements;
        vector.m_capacity = initial_capacity;
        vector.m_count = 0;
        return vector;
    }

    ALWAYS_INLINE static ErrorOr<Vector> create_from_span(Span<const T> element_span)
    {
        TRY_ASSIGN(Vector vector, create_with_initial_capacity(element_span.count()));
        vector.m_count = element_span.count();
        copy_elements(vector.m_elements, element_span.elements(), element_span.count());
        return vector;
    }

    ALWAYS_INLINE static ErrorOr<Vector> create_filled(usize initial_count)
    {
        TRY_ASSIGN(Vector vector, create_with_initial_capacity(initial_count));
        vector.m_count = initial_count;
        for (usize index = 0; index < vector.m_count; ++index) {
            new (vector.m_elements + index) T();
        }
        return vector;
    }

    ALWAYS_INLINE static ErrorOr<Vector> create_filled(usize initial_count, const T& template_element)
    {
        TRY_ASSIGN(Vector vector, create_with_initial_capacity(initial_count));
        vector.m_count = initial_count;
        for (usize index = 0; index < vector.m_count; ++index) {
            new (vector.m_elements + index) T(template_element);
        }
        return vector;
    }

    ALWAYS_INLINE static ErrorOr<void> assign(Vector& self, const Vector& other)
    {
        self.clear();
        TRY(self.allocate_new_if_required(other.m_count));
        self.m_count = other.m_count;

        copy_elements(self.m_elements, other.m_elements, other.m_count);
        return {};
    }

public:
    ALWAYS_INLINE Vector()
        : m_elements(nullptr)
        , m_capacity(0)
        , m_count(0)
    {}

    ALWAYS_INLINE Vector(const Vector& other)
    {
        MUST_ASSIGN(Vector copied_vector, create(other));
        // NOTE: Invoking the move constructor is extremely cheap as there are only a few
        //       raw pointer assignments and no memory allocation or object initialization.
        new (this) Vector(move(copied_vector));
    }

    ALWAYS_INLINE Vector(Vector&& other) noexcept
        : m_elements(other.m_elements)
        , m_capacity(other.m_capacity)
        , m_count(other.m_count)
    {
        other.m_elements = nullptr;
        other.m_capacity = 0;
        other.m_count = 0;
    }

    ALWAYS_INLINE Vector& operator=(const Vector& other)
    {
        MUST(assign(*this, other));
        return *this;
    }

    ALWAYS_INLINE Vector& operator=(Vector&& other) noexcept
    {
        clear_and_shrink();

        m_elements = other.m_elements;
        m_capacity = other.m_capacity;
        m_count = other.m_count;

        other.m_elements = nullptr;
        other.m_capacity = 0;
        other.m_count = 0;

        return *this;
    }

    ALWAYS_INLINE ~Vector()
    {
        clear();
        MUST(release_memory(m_elements, m_capacity));
    }

public:
    NODISCARD ALWAYS_INLINE T* elements() { return m_elements; }
    NODISCARD ALWAYS_INLINE const T* elements() const { return m_elements; }

    NODISCARD ALWAYS_INLINE T* operator*() { return elements(); }
    NODISCARD ALWAYS_INLINE const T* operator*() const { return elements(); }

    NODISCARD ALWAYS_INLINE usize capacity() const { return m_capacity; }
    NODISCARD ALWAYS_INLINE usize count() const { return m_count; }
    NODISCARD ALWAYS_INLINE static constexpr usize element_size() { return sizeof(T); }

    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_count == 0); }
    NODISCARD ALWAYS_INLINE bool has_elements() const { return (m_count > 0); }

    NODISCARD ALWAYS_INLINE Span<T> span() { return Span<T>(m_elements, m_count); }
    NODISCARD ALWAYS_INLINE Span<const T> span() const { return Span<const T>(m_elements, m_count); }

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

    NODISCARD ALWAYS_INLINE T& first() { return at(0); }
    NODISCARD ALWAYS_INLINE const T& first() const { return at(0); }

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
        AT_ASSERT(offset + count <= m_count);
        return Span<T>(m_elements + offset, count);
    }

    NODISCARD ALWAYS_INLINE Span<const T> slice(usize offset, usize count) const
    {
        AT_ASSERT(offset + count <= m_count);
        return Span<const T>(m_elements + offset, count);
    }

public:
    template<typename... Args>
    ALWAYS_INLINE ErrorOr<void> try_emplace(Args&&... args)
    {
        TRY(re_allocate_if_required(m_count + 1));
        new (m_elements + m_count) T(forward<Args>(args)...);
        ++m_count;
        return {};
    }

    template<typename... Args>
    ALWAYS_INLINE void emplace(Args&&... args)
    {
        MUST(try_emplace(forward<Args>(args)...));
    }

    ALWAYS_INLINE ErrorOr<void> try_add(const T& element)
    {
        // NOTE: It will invoke the copy constructor.
        TRY(try_emplace(element));
        return {};
    }

    ALWAYS_INLINE ErrorOr<void> try_add(T&& element)
    {
        // NOTE: It will invoke the move constructor.
        TRY(try_emplace(move(element)));
        return {};
    }

    ALWAYS_INLINE void add(const T& element) { MUST(try_emplace(element)); }
    ALWAYS_INLINE void add(T&& element) { MUST(try_emplace(move(element))); }

    ALWAYS_INLINE ErrorOr<void> try_add_span(Span<const T> elements)
    {
        TRY(re_allocate_if_required(m_count + elements.count()));
        copy_elements(m_elements + m_count, elements.elements(), elements.count());
        m_count += elements.count();
        return {};
    }

    ALWAYS_INLINE void add_span(Span<const T> elements) { MUST(try_add_span(elements)); }

public:
    ALWAYS_INLINE void remove_last()
    {
        AT_ASSERT(has_elements());
        m_elements[--m_count].~T();
    }

    ALWAYS_INLINE void remove_last(usize count)
    {
        AT_ASSERT(m_count >= count);
        const usize remove_offset = m_count - count;
        for (usize index = 0; index < count; ++index) {
            m_elements[remove_offset + index].~T();
        }
        m_count -= count;
    }

    ALWAYS_INLINE ErrorOr<void> remove_last_and_shrink()
    {
        remove_last();
        TRY(shrink_to_fit());
        return {};
    }

    ALWAYS_INLINE ErrorOr<void> remove_last_and_shrink(usize count)
    {
        remove_last(count);
        TRY(shrink_to_fit());
        return {};
    }

public:
    ALWAYS_INLINE void clear()
    {
        for (usize index = 0; index < m_count; ++index) {
            m_elements[index].~T();
        }
        m_count = 0;
    }

    ALWAYS_INLINE ErrorOr<void> shrink_to_fit()
    {
        if (m_capacity > m_count) {
            TRY(re_allocate_to_fixed(m_count));
        }
        return {};
    }

    ALWAYS_INLINE void clear_and_shrink()
    {
        clear();
        MUST(release_memory(m_elements, m_capacity));
        m_elements = nullptr;
        m_capacity = 0;
    }

    //
    // Ensures that the capacity of the vector will be at least equal to the given value.
    // The vector only grows if necessary and there are no guarantees that the new capacity
    // will be exactly equal to required_capacity.
    //
    ALWAYS_INLINE ErrorOr<void> ensure_capacity(usize required_capacity)
    {
        TRY(re_allocate_if_required(required_capacity));
        return {};
    }

    //
    // Ensures that the capacity of the vector will be at least equal to the given value.
    // The vector only grows if necessary and if so the new capacity will be equal to fixed_required_capacity.
    //
    ALWAYS_INLINE ErrorOr<void> ensure_fixed_capacity(usize fixed_required_capacity)
    {
        if (m_capacity < fixed_required_capacity) {
            TRY(re_allocate_to_fixed(fixed_required_capacity));
        }
        return {};
    }

    //
    // The given capacity must be sufficient to store the elements currently held by the container.
    //
    ALWAYS_INLINE ErrorOr<void> set_fixed_capacity(usize fixed_capacity)
    {
        if (fixed_capacity == m_capacity) {
            // No action needed. Calling re_allocate_to_fixed() in this case would actually trigger an assertion.
            return {};
        }

        AT_ASSERT(fixed_capacity >= m_count);
        TRY(re_allocate_to_fixed(fixed_capacity));
        return {};
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
    NODISCARD ALWAYS_INLINE static ErrorOr<T*> allocate_memory(usize capacity)
    {
        // TODO: Make sure that capacity * sizeof(T) doesn't overflow.
        void* memory_block = ::operator new(capacity * sizeof(T));

        if (memory_block == nullptr) {
            return Error::OutOfMemory;
        }
        return reinterpret_cast<T*>(memory_block);
    }

    ALWAYS_INLINE static ErrorOr<void> release_memory(T* elements, usize capacity)
    {
        // NOTE: The standard operator delete doesn't need the capacity of the memory block.
        //       However, in future implementations we might switch to a custom memory allocator,
        //       so having this crucial information available out-of-the-box is really handy.
        (void)capacity;

        ::operator delete(elements);
        return {};
    }

    ALWAYS_INLINE static void copy_elements(T* destination, const T* source, usize count)
    {
        for (usize index = 0; index < count; ++index) {
            new (destination + index) T(source[index]);
        }
    }

    ALWAYS_INLINE static void move_elements(T* destination, T* source, usize count)
    {
        for (usize index = 0; index < count; ++index) {
            new (destination + index) T(move(source[index]));
            source[index].~T();
        }
    }

private:
    //
    NODISCARD ALWAYS_INLINE usize get_next_capacity(usize required_capacity) const
    {
        const usize geometric_capacity = m_capacity + m_capacity / 2;
        if (geometric_capacity < required_capacity) {
            return required_capacity;
        }
        return geometric_capacity;
    }

    ALWAYS_INLINE ErrorOr<void> allocate_new_to_fixed(usize new_capacity)
    {
        // NOTE: These assertions are triggered only by a bug in the Vector implementation.
        //       No user action/command *should* trigger them.
        AT_ASSERT(new_capacity >= m_count);
        AT_ASSERT(new_capacity != m_capacity);

        clear();
        TRY(release_memory(m_elements, m_capacity));

        m_capacity = new_capacity;
        TRY_ASSIGN(m_elements, allocate_memory(m_capacity));
        return {};
    }

    ALWAYS_INLINE ErrorOr<void> re_allocate_to_fixed(usize new_capacity)
    {
        // NOTE: These assertions are triggered only by a bug in the Vector implementation.
        //       No user action/command *should* trigger them.
        AT_ASSERT(new_capacity >= m_count);
        AT_ASSERT(new_capacity != m_capacity);

        TRY_ASSIGN(auto new_elements, allocate_memory(new_capacity));
        move_elements(new_elements, m_elements, m_count);
        TRY(release_memory(m_elements, m_capacity));

        m_elements = new_elements;
        m_capacity = new_capacity;
        return {};
    }

    ALWAYS_INLINE ErrorOr<void> re_allocate_if_required(usize required_capacity)
    {
        if (required_capacity > m_capacity) {
            const usize new_capacity = get_next_capacity(required_capacity);
            TRY(re_allocate_to_fixed(new_capacity));
        }
        return {};
    }

    ALWAYS_INLINE ErrorOr<void> allocate_new_if_required(usize required_capacity)
    {
        if (required_capacity > m_capacity) {
            const usize new_capacity = get_next_capacity(required_capacity);
            TRY(allocate_new_to_fixed(new_capacity));
        }
        return {};
    }

private:
    T* m_elements;
    usize m_capacity;
    usize m_count;
};

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::Vector;
#endif // AT_INCLUDE_GLOBALLY