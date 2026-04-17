/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/ArrayIterator.h"
#include "AND/Assertions.h"
#include "AND/IterationDecision.h"
#include "AND/MatchResult.h"
#include "AND/Span.h"
#include "AND/Types.h"

#include <initializer_list>
#include <new>

namespace AND {

template<typename StorageType, usize inline_capacity>
union VectorStorage {
    NODISCARD ALWAYS_INLINE constexpr StorageType* inlined() { return inline_storage; }
    NODISCARD ALWAYS_INLINE constexpr const StorageType* inlined() const { return inline_storage; }

    StorageType* heap_elements;
    alignas(StorageType) u8 inline_storage[inline_capacity * sizeof(StorageType)];
};

template<typename StorageType>
union VectorStorage<StorageType, 0> {
    NODISCARD ALWAYS_INLINE static constexpr StorageType* inlined() { return nullptr; }

    StorageType* heap_elements;
};

template<typename T, usize inline_capacity = 0>
requires(!is_rvalue_reference<T>)
class Vector {
public:
    template<typename Q, usize other_inline_capacity>
    requires(!is_rvalue_reference<Q>)
    friend class Vector;

    using StorageType = RemoveReference<T>;
    static constexpr bool is_reference_vector = is_reference<T>;

    static constexpr usize growth_factor_numerator = 3;
    static constexpr usize growth_factor_denominator = 2;
    static_assert(growth_factor_numerator > growth_factor_denominator && growth_factor_denominator > 0);

    using Iterator = ArrayIterator<StorageType>;
    using ConstIterator = ArrayIterator<StorageType const>;
    using ReverseIterator = ArrayReverseIterator<StorageType>;
    using ReverseConstIterator = ArrayReverseIterator<StorageType const>;

    struct ScopedReentrantBlock {
    public:
        explicit ScopedReentrantBlock(Vector const& vector)
            : m_vector(vector)
        {
            m_vector.block_reentrant_calls();
        }

        ~ScopedReentrantBlock()
        {
            m_vector.unblock_reentrant_calls();
        }

    private:
        Vector const& m_vector;
    };

#define AND_VECTOR_SCOPED_REENTRANT_BLOCK \
    auto AND_CONCAT(scoped, AND_LINE) = ScopedReentrantBlock(*this)

#define AND_VECTOR_CHECK_IF_UNBLOCKED \
    ASSERTF(m_block_reentrant_calls == false, "A re-entrant call in the Vector container was detected!")

public:
    NODISCARD ALWAYS_INLINE static Vector from_initial_capacity(usize initial_capacity)
    {
        Vector vector;
        vector.ensure_capacity(initial_capacity);
        return vector;
    }

    NODISCARD ALWAYS_INLINE static Vector from_span(Span<T const> elements)
    {
        Vector vector = Vector::from_initial_capacity(elements.count());
        vector.m_count = elements.count();
        Vector::copy_elements(vector.elements(), elements.elements(), vector.m_count);
        return vector;
    }

    NODISCARD ALWAYS_INLINE static Vector from_init_list(std::initializer_list<T> elements)
    {
        auto span = Span<T const>(elements.begin(), elements.size());
        return Vector::from_span(span);
    }

public:
    ALWAYS_INLINE Vector()
        : m_count(0)
        , m_capacity(inline_capacity)
        , m_storage {}
    {
    }

    ALWAYS_INLINE ~Vector()
    {
        clear();
        shrink_to_fit();
    }

    ALWAYS_INLINE Vector(Vector const& other)
        : m_count(0)
        , m_capacity(inline_capacity)
        , m_storage {}
    {
        ensure_capacity(other.m_count);
        m_count = other.m_count;

        {
            AND_VECTOR_SCOPED_REENTRANT_BLOCK;
            Vector::copy_elements(elements(), other.elements(), m_count);
        }
    }

    ALWAYS_INLINE Vector(Vector&& other) noexcept
        : m_count(other.m_count)
        , m_capacity(other.m_capacity)
    {
        if (other.is_stored_inline()) {
            AND_VECTOR_SCOPED_REENTRANT_BLOCK;
            Vector::move_elements(m_storage.inlined(), other.m_storage.inlined(), m_count);
        } else {
            m_storage.heap_elements = other.m_storage.heap_elements;
            other.m_storage.heap_elements = nullptr;
        }

        other.m_capacity = inline_capacity;
    }

    ALWAYS_INLINE Vector& operator=(Vector const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        clear();
        ensure_capacity(other.m_count);
        m_count = other.m_count;
        {
            AND_VECTOR_SCOPED_REENTRANT_BLOCK;
            Vector::copy_elements(elements(), other.elements(), other.m_count);
        }

        return *this;
    }

    ALWAYS_INLINE Vector& operator=(Vector&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        clear();
        shrink_to_fit();

        m_count = other.m_count;
        m_capacity = other.m_capacity;
        if (other.is_stored_inline()) {
            AND_VECTOR_SCOPED_REENTRANT_BLOCK;
            Vector::move_elements(m_storage.inlined(), other.m_storage.inlined(), m_count);
        } else {
            m_storage.heap_elements = other.m_storage.heap_elements;
            other.m_storage.heap_elements = nullptr;
        }

        other.m_capacity = inline_capacity;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE StorageType* elements() { return is_stored_inline() ? m_storage.inlined() : m_storage.heap_elements; }
    NODISCARD ALWAYS_INLINE StorageType const* elements() const { return is_stored_inline() ? m_storage.inlined() : m_storage.heap_elements; }

    NODISCARD ALWAYS_INLINE usize capacity() const { return m_capacity; }
    NODISCARD ALWAYS_INLINE usize count() const { return m_count; }
    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_count == 0); }
    NODISCARD ALWAYS_INLINE bool has_elements() const { return (m_count > 0); }

    NODISCARD ALWAYS_INLINE Span<StorageType> span() { return Span<StorageType>(elements(), m_count); }
    NODISCARD ALWAYS_INLINE Span<StorageType const> span() const { return Span<StorageType const>(elements(), m_count); }

public:
    NODISCARD ALWAYS_INLINE T& operator[](usize index)
    {
        ASSERTF(index < m_count, "Index out-of-range in Vector::operator[]()!");
        AND_VECTOR_CHECK_IF_UNBLOCKED;
        return elements()[index];
    }

    NODISCARD ALWAYS_INLINE T const& operator[](usize index) const
    {
        ASSERTF(index < m_count, "Index out-of-range in Vector::operator[]()!");
        AND_VECTOR_CHECK_IF_UNBLOCKED;
        return elements()[index];
    }

    NODISCARD ALWAYS_INLINE T& first()
    {
        ASSERTF(has_elements(), "Trying to access first element from an empty vector (in Vector::first())!");
        AND_VECTOR_CHECK_IF_UNBLOCKED;
        return elements()[0];
    }

    NODISCARD ALWAYS_INLINE const T& first() const
    {
        ASSERTF(has_elements(), "Trying to access first element from an empty vector (in Vector::first())!");
        AND_VECTOR_CHECK_IF_UNBLOCKED;
        return elements()[0];
    }

    NODISCARD ALWAYS_INLINE T& last()
    {
        ASSERTF(has_elements(), "Trying to access last element from an empty vector (in Vector::last())!");
        AND_VECTOR_CHECK_IF_UNBLOCKED;
        return elements()[m_count - 1];
    }

    NODISCARD ALWAYS_INLINE const T& last() const
    {
        ASSERTF(has_elements(), "Trying to access last element from an empty vector (in Vector::last())!");
        AND_VECTOR_CHECK_IF_UNBLOCKED;
        return elements()[m_count - 1];
    }

public:
    ALWAYS_INLINE void push_back(T const& value)
    {
        ensure_capacity(m_count + 1);
        {
            AND_VECTOR_SCOPED_REENTRANT_BLOCK;
            new (elements() + m_count) StorageType(value);
        }
        ++m_count;
    }

    ALWAYS_INLINE void push_back(T&& value)
    {
        ensure_capacity(m_count + 1);
        {
            AND_VECTOR_SCOPED_REENTRANT_BLOCK;
            new (elements() + m_count) StorageType(move(value));
        }
        ++m_count;
    }

    template<typename... Args>
    ALWAYS_INLINE T& emplace_back(Args&&... args)
    {
        ensure_capacity(m_count + 1);
        {
            AND_VECTOR_SCOPED_REENTRANT_BLOCK;
            new (elements() + m_count) StorageType(forward<Args>(args)...);
        }
        return elements()[m_count++];
    }

    ALWAYS_INLINE void remove_last()
    {
        ASSERTF(has_elements(), "Trying to pop back from an empty vector (in Vector::pop_back())!");
        AND_VECTOR_SCOPED_REENTRANT_BLOCK;
        elements()[--m_count].~T();
    }

    NODISCARD ALWAYS_INLINE T take_last()
    {
        ASSERTF(has_elements(), "Trying to take the last element from an empty vector (in Vector::take_last())!");

        // We can't use a scoped reentrant block because we need the 'element' variable
        // later when we return from the function, and we can't block the entire scope
        // because 'remove_last' also tries to block the container.
        block_reentrant_calls();
        T element = move(last());
        unblock_reentrant_calls();

        remove_last();
        return element;
    }

    ALWAYS_INLINE void remove_at(usize index)
    {
        ASSERTF(index < m_count, "Index out-of-range in Vector::remove_at()!");
        AND_VECTOR_SCOPED_REENTRANT_BLOCK;

        elements()[index].~T();
        move_elements(elements() + index, elements() + index + 1, m_count - index - 1);
        --m_count;
    }

    NODISCARD ALWAYS_INLINE T take_index(usize index)
    {
        ASSERTF(index < m_count, "Index out-of-range in Vector::take_index()!");

        // NOTE: Check out 'take_last' for more information about this pattern and why we don't
        // use a scoped reentrant block.
        block_reentrant_calls();
        T element = move(elements()[index]);
        unblock_reentrant_calls();

        remove_at(index);
        return element;
    }

    ALWAYS_INLINE void remove_at_unordered(usize index)
    {
        ASSERTF(index < m_count, "Index out-of-range in Vector::remove_at_unordered()!");

        {
            AND_VECTOR_SCOPED_REENTRANT_BLOCK;
            elements()[index].~T();
        }

        if (index != m_count - 1) {
            new (elements() + index) StorageType(move(last()));
            remove_last();
        }
        --m_count;
    }

    NODISCARD ALWAYS_INLINE T take_index_unordered(usize index)
    {
        ASSERTF(index < m_count, "Index out-of-range in Vector::take_index_unordered()!");

        // NOTE: Check out 'take_last' for more information about this pattern and why we don't
        // use a scoped reentrant block.
        block_reentrant_calls();
        T element = move(elements()[index]);
        unblock_reentrant_calls();

        remove_at_unordered(index);
        return element;
    }

public:
    ALWAYS_INLINE void clear()
    {
        AND_VECTOR_SCOPED_REENTRANT_BLOCK;
        StorageType* stored_elements = elements();
        for (usize index = 0; index < m_count; ++index)
            stored_elements[index].~T();
        m_count = 0;
    }

    ALWAYS_INLINE void shrink_to_fit()
    {
        AND_VECTOR_SCOPED_REENTRANT_BLOCK;

        if (m_capacity == m_count || is_stored_inline())
            return;

        if (could_fit_inline()) {
            StorageType* old_elements = m_storage.heap_elements;
            Vector::move_elements(m_storage.heap_elements, old_elements, m_count);
            Vector::free_memory(old_elements, m_capacity);
            m_capacity = inline_capacity;
        } else {
            usize new_capacity = m_count;
            StorageType* new_elements = Vector::allocate_memory(new_capacity);
            Vector::move_elements(new_elements, m_storage.heap_elements, m_count);
            Vector::free_memory(m_storage.heap_elements, m_capacity);

            m_storage.heap_elements = new_elements;
            m_capacity = new_capacity;
        }
    }

    ALWAYS_INLINE void ensure_capacity(usize minimum_capacity)
    {
        AND_VECTOR_SCOPED_REENTRANT_BLOCK;

        if (m_capacity >= minimum_capacity)
            return;

        usize new_capacity = Vector::calculate_next_capacity(m_capacity);
        if (new_capacity < minimum_capacity)
            new_capacity = minimum_capacity;
        ASSERT(new_capacity > inline_capacity);

        StorageType* new_elements = Vector::allocate_memory(new_capacity);
        Vector::move_elements(new_elements, elements(), m_count);

        if (is_stored_on_heap())
            Vector::free_memory(m_storage.heap_elements, m_capacity);

        m_capacity = new_capacity;
        m_storage.heap_elements = new_elements;
    }

public:
    NODISCARD ALWAYS_INLINE bool contains(T const& element) const
    {
        AND_VECTOR_CHECK_IF_UNBLOCKED;

        for (usize index = 0; index < m_count; ++index) {
            if (elements()[index] == element)
                return true;
        }
        return false;
    }

    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each(Predicate predicate)
    {
        AND_VECTOR_CHECK_IF_UNBLOCKED;

        T* stored_elements = elements();
        for (usize index = 0; index < m_count; ++index) {
            StorageType& element = stored_elements[index];
            if (predicate(element) == IterationDecision::Break)
                return IterationDecision::Break;
        }
        return IterationDecision::Continue;
    }

    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each(Predicate predicate) const
    {
        AND_VECTOR_CHECK_IF_UNBLOCKED;

        T const* stored_elements = elements();
        for (usize index = 0; index < m_count; ++index) {
            StorageType const& element = stored_elements[index];
            if (predicate(element) == IterationDecision::Break)
                return IterationDecision::Break;
        }
        return IterationDecision::Continue;
    }

    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each_with_index(Predicate predicate)
    {
        AND_VECTOR_CHECK_IF_UNBLOCKED;

        T* stored_elements = elements();
        for (usize index = 0; index < m_count; ++index) {
            StorageType& element = stored_elements[index];
            if (predicate(element, index) == IterationDecision::Break)
                return IterationDecision::Break;
        }
        return IterationDecision::Continue;
    }

    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each_with_index(Predicate predicate) const
    {
        AND_VECTOR_CHECK_IF_UNBLOCKED;

        T const* stored_elements = elements();
        for (usize index = 0; index < m_count; ++index) {
            StorageType const& element = stored_elements[index];
            if (predicate(element, index) == IterationDecision::Break)
                return IterationDecision::Break;
        }
        return IterationDecision::Continue;
    }

    template<typename MatchPredicate>
    ALWAYS_INLINE void remove_all_matching(MatchPredicate match_predicate)
    {
        AND_VECTOR_CHECK_IF_UNBLOCKED;

        T* stored_elements = elements();
        usize index = 0;
        while (index < m_count) {
            StorageType& element = stored_elements[index];
            if (match_predicate(element) == MatchResult::Yes)
                remove_at(index);
            else
                ++index;
        }
    }

    template<typename MatchPredicate>
    ALWAYS_INLINE void remove_all_matching_unordered(MatchPredicate match_predicate)
    {
        AND_VECTOR_CHECK_IF_UNBLOCKED;

        T* stored_elements = elements();
        usize index = 0;
        while (index < m_count) {
            StorageType& element = stored_elements[index];
            if (match_predicate(element) == MatchResult::Yes)
                remove_at_unordered(index);
            else
                ++index;
        }
    }

    ALWAYS_INLINE void remove_all(T const& element)
    {
        remove_all_matching([&](auto const& other) {
            return (element == other)
                ? MatchResult::Yes
                : MatchResult::No;
        });
    }

    ALWAYS_INLINE void remove_all_unordered(T const& element)
    {
        remove_all_matching_unordered([&](auto const& other) {
            return (element == other)
                ? MatchResult::Yes
                : MatchResult::No;
        });
    }

public:
    NODISCARD ALWAYS_INLINE Iterator begin() { return Iterator(elements()); }
    NODISCARD ALWAYS_INLINE Iterator end() { return Iterator(elements() + m_count); }

    NODISCARD ALWAYS_INLINE ConstIterator begin() const { return ConstIterator(elements()); }
    NODISCARD ALWAYS_INLINE ConstIterator end() const { return ConstIterator(elements() + m_count); }

    NODISCARD ALWAYS_INLINE ReverseIterator rbegin() { return ReverseIterator(elements() + m_count - 1); }
    NODISCARD ALWAYS_INLINE ReverseIterator rend() { return ReverseIterator(elements() - 1); }

    NODISCARD ALWAYS_INLINE ReverseConstIterator rbegin() const { return ReverseConstIterator(elements() + m_count - 1); }
    NODISCARD ALWAYS_INLINE ReverseConstIterator rend() const { return ReverseConstIterator(elements() - 1); }

private:
    NODISCARD ALWAYS_INLINE constexpr bool is_stored_inline() const { return (m_capacity <= inline_capacity); }
    NODISCARD ALWAYS_INLINE constexpr bool is_stored_on_heap() const { return (m_capacity > inline_capacity); }
    NODISCARD ALWAYS_INLINE constexpr bool could_fit_inline() const { return (m_count <= inline_capacity); }

    NODISCARD ALWAYS_INLINE static usize calculate_next_capacity(usize current_capacity)
    {
        // TODO: Make sure that the multiplication doesn't overflow!
        return current_capacity * growth_factor_numerator / growth_factor_denominator;
    }

    NODISCARD ALWAYS_INLINE static StorageType* allocate_memory(usize in_capacity)
    {
        if (in_capacity == 0)
            return nullptr;

        usize allocation_size = in_capacity * sizeof(StorageType);
        void* memory = ::operator new(allocation_size);
        return static_cast<StorageType*>(memory);
    }

    ALWAYS_INLINE static void free_memory(StorageType* elements, usize capacity)
    {
        if (capacity == 0)
            return;

        MAYBE_UNUSED usize allocation_size = capacity * sizeof(StorageType);
        ::operator delete(elements);
    }

    ALWAYS_INLINE static void copy_elements(StorageType* destination, StorageType const* source, usize in_count)
    {
        for (usize index = 0; index < in_count; ++index)
            new (destination + index) StorageType(source[index]);
    }

    ALWAYS_INLINE static void move_elements(StorageType* destination, StorageType* source, usize in_count)
    {
        for (usize index = 0; index < in_count; ++index) {
            new (destination + index) StorageType(move(source[index]));
            source[index].~T();
        }
    }

    ALWAYS_INLINE void block_reentrant_calls() const
    {
        ASSERTF(m_block_reentrant_calls == false, "A re-entrant call in the Vector container was detected!");
        m_block_reentrant_calls = true;
    }

    ALWAYS_INLINE void unblock_reentrant_calls() const
    {
        ASSERT(m_block_reentrant_calls == true);
        m_block_reentrant_calls = false;
    }

private:
    usize m_count;
    usize m_capacity;
    VectorStorage<StorageType, inline_capacity> m_storage;

    // Reentrancy guard for internal mutation-sensitive sections.
    // Set to true while operations perform element construction/destruction/moves,
    // where callbacks or user code must not call back into this same container.
    // This is a debug correctness guard, not a synchronization/thread-safety mechanism.
    mutable bool m_block_reentrant_calls { false };
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::Vector;
#endif
