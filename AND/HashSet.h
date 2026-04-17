/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Assertions.h"
#include "AND/Hash.h"
#include "AND/IterationDecision.h"
#include "AND/MatchResult.h"
#include "AND/OpenAddressingBucketState.h"
#include "AND/Optional.h"
#include "AND/Span.h"
#include "AND/Types.h"

#include <initializer_list>
#include <new>

namespace AND {

enum class HashSetAdd : u8 {
    DidAdd,
    AlreadyExists,
};

enum class HashSetRemove : u8 {
    DidRemove,
    NotFound,
};

template<typename T>
requires(!is_const<T> && !is_reference<T>)
class HashSet {
public:
    template<typename Q>
    requires(!is_const<Q> && !is_reference<Q>)
    friend class HashSet;

    using BucketState = OpenAddressingBucketState;

    static constexpr usize max_load_factor_numerator = 3;
    static constexpr usize max_load_factor_denominator = 4;
    static_assert(max_load_factor_numerator <= max_load_factor_denominator && max_load_factor_numerator > 0);

    struct ScopedReentrantBlock {
    public:
        explicit ScopedReentrantBlock(HashSet const& hash_set)
            : m_hash_set(hash_set)
        {
            m_hash_set.block_reentrant_calls();
        }

        ~ScopedReentrantBlock()
        {
            m_hash_set.unblock_reentrant_calls();
        }

    private:
        HashSet const& m_hash_set;
    };

#define AND_HASH_SET_SCOPED_REENTRANT_BLOCK \
    auto AND_CONCAT(scoped, AND_LINE) = ScopedReentrantBlock(*this)

#define AND_HASH_SET_CHECK_IF_UNBLOCKED \
    ASSERTF(m_block_reentrant_calls == false, "A re-entrant call in the HashSet container was detected!")

public:
    NODISCARD ALWAYS_INLINE static HashSet from_initial_capacity(usize initial_capacity)
    {
        HashSet hash_set;
        hash_set.m_bucket_count = calculate_bucket_count(initial_capacity);
        HashSet::allocate_memory(hash_set.m_bucket_count, hash_set.m_buckets, hash_set.m_bucket_states);
        for (usize index = 0; index < hash_set.m_bucket_count; ++index)
            hash_set.m_bucket_states[index] = BucketState::empty();

        return hash_set;
    }

    NODISCARD ALWAYS_INLINE static HashSet from_init_list(std::initializer_list<T> init_list)
    {
        HashSet hash_set = HashSet::from_initial_capacity(init_list.size());
        for (auto const& element : init_list)
            hash_set.add(element);
        return hash_set;
    }

    NODISCARD ALWAYS_INLINE static HashSet from_span(Span<T> span)
    {
        HashSet hash_set = HashSet::from_initial_capacity(span.count());
        for (auto const& element : span)
            hash_set.add(element);
        return hash_set;
    }

public:
    ALWAYS_INLINE HashSet()
        : m_count(0)
        , m_bucket_count(0)
        , m_buckets(nullptr)
        , m_bucket_states(nullptr)
    {
    }

    ALWAYS_INLINE ~HashSet()
    {
        clear_and_shrink();
    }

    ALWAYS_INLINE HashSet(HashSet const& other)
        : m_count(0)
        , m_bucket_count(0)
        , m_buckets(nullptr)
        , m_bucket_states(nullptr)
    {
        ensure_capacity(other.m_count);
        other.for_each([&](T const& element) { add(element); return IterationDecision::Continue; });
    }

    ALWAYS_INLINE HashSet(HashSet&& other) noexcept
        : m_count(other.m_count)
        , m_bucket_count(other.m_bucket_count)
        , m_buckets(other.m_buckets)
        , m_bucket_states(other.m_bucket_states)
    {
        other.m_count = 0;
        other.m_bucket_count = 0;
        other.m_buckets = nullptr;
        other.m_bucket_states = nullptr;
    }

    ALWAYS_INLINE HashSet& operator=(HashSet const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        clear();
        ensure_capacity(other.m_count);
        other.for_each([&](T const& element) { add(element); return IterationDecision::Continue; });

        return *this;
    }

    ALWAYS_INLINE HashSet& operator=(HashSet&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        clear_and_shrink();

        m_count = other.m_count;
        m_bucket_count = other.m_bucket_count;
        m_buckets = other.m_buckets;
        m_bucket_states = other.m_bucket_states;

        other.m_count = 0;
        other.m_bucket_count = 0;
        other.m_buckets = nullptr;
        other.m_bucket_states = nullptr;

        return *this;
    }

public:
    // FIXME: There should be two overloads of this function, one that takes 'T const&' and another
    //        one that takes 'T&&' instead of a single one that takes 'T' in order to avoid redundant copies.
    ALWAYS_INLINE HashSetAdd add(T element)
    {
        AND_HASH_SET_CHECK_IF_UNBLOCKED;
        ensure_capacity(m_count + 1);

        Hash hash = get_hash(element);
        usize base_index = hash % m_bucket_count;
        Optional<usize> destination_index;

        for (usize iteration = 0; iteration < m_bucket_count; ++iteration) {
            usize index = (base_index + iteration) % m_bucket_count;
            auto state = m_bucket_states[index];

            if (state.is_occupied()) {
                if (state.matches_hash(hash) && element == m_buckets[index])
                    return HashSetAdd::AlreadyExists;
            } else {
                if (!destination_index.has_value())
                    destination_index = index;

                if (!state.was_ever_occupied())
                    break;
            }
        }

        {
            AND_HASH_SET_SCOPED_REENTRANT_BLOCK;
            ASSERT(destination_index.has_value());
            usize index = destination_index.value();
            new (m_buckets + index) T(move(element));
            m_bucket_states[index] = BucketState::occupied(hash);
            ++m_count;
        }

        return HashSetAdd::DidAdd;
    }

    ALWAYS_INLINE HashSetRemove remove(T const& element)
    {
        AND_HASH_SET_CHECK_IF_UNBLOCKED;

        // Avoid division (modulo operation) by zero.
        if (m_bucket_count == 0)
            return HashSetRemove::NotFound;

        Hash hash = get_hash(element);
        usize base_index = hash % m_bucket_count;

        for (usize iteration = 0; iteration < m_bucket_count; ++iteration) {
            usize index = (base_index + iteration) % m_bucket_count;
            auto& state = m_bucket_states[index];

            if (state.was_ever_occupied()) {
                if (state.is_occupied() && state.matches_hash(hash) && element == m_buckets[index]) {
                    AND_HASH_SET_SCOPED_REENTRANT_BLOCK;
                    m_buckets[index].~T();
                    state = BucketState::tombstone();
                    --m_count;

                    return HashSetRemove::DidRemove;
                }
            } else {
                // We can be certain that the element does not exist in the set.
                return HashSetRemove::NotFound;
            }
        }

        return HashSetRemove::NotFound;
    }

    ALWAYS_INLINE bool contains(T const& element) const
    {
        AND_HASH_SET_CHECK_IF_UNBLOCKED;

        // Avoid division (modulo operation) by zero.
        if (m_bucket_count == 0)
            return false;

        Hash hash = get_hash(element);
        usize base_index = hash % m_bucket_count;

        for (usize iteration = 0; iteration < m_bucket_count; ++iteration) {
            usize index = (base_index + iteration) % m_bucket_count;
            auto state = m_bucket_states[index];

            if (state.was_ever_occupied()) {
                if (state.is_occupied() && state.matches_hash(hash) && element == m_buckets[index])
                    return true;
            } else {
                // We can be certain that the element does not exist in the set.
                return false;
            }
        }

        // We iterated over all the buckets and still haven't found the element.
        return false;
    }

public:
    ALWAYS_INLINE void clear()
    {
        AND_HASH_SET_SCOPED_REENTRANT_BLOCK;

        for (usize index = 0; index < m_bucket_count; ++index) {
            if (m_bucket_states[index].is_occupied())
                m_buckets[index].~T();
            m_bucket_states[index] = BucketState::empty();
        }
        m_count = 0;
    }

    ALWAYS_INLINE void clear_and_shrink()
    {
        AND_HASH_SET_SCOPED_REENTRANT_BLOCK;

        for_each_internal([](T& element) { element.~T(); return IterationDecision::Continue; });
        HashSet::free_memory(m_buckets, m_bucket_count);
        m_count = 0;
        m_bucket_count = 0;
        m_buckets = nullptr;
        m_bucket_states = nullptr;
    }

    ALWAYS_INLINE void ensure_capacity(usize minimum_capacity)
    {
        AND_HASH_SET_CHECK_IF_UNBLOCKED;
        usize min_bucket_count = calculate_bucket_count(minimum_capacity);
        if (min_bucket_count <= m_bucket_count)
            return;

        HashSet new_hash_set = HashSet::from_initial_capacity(minimum_capacity);
        for_each([&](T const& element) { new_hash_set.add(element); return IterationDecision::Continue; });
        *this = move(new_hash_set);
    }

public:
    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each(Predicate predicate) const
    {
        AND_HASH_SET_CHECK_IF_UNBLOCKED;
        for (usize index = 0; index < m_bucket_count; ++index) {
            if (m_bucket_states[index].is_occupied()) {
                T const& element = m_buckets[index];
                if (predicate(element) == IterationDecision::Break)
                    return IterationDecision::Break;
            }
        }
        return IterationDecision::Continue;
    }

    template<typename MatchPredicate>
    ALWAYS_INLINE void remove_all_matching(MatchPredicate match_predicate)
    {
        AND_HASH_SET_SCOPED_REENTRANT_BLOCK;
        for (usize index = 0; index < m_bucket_count; ++index) {
            if (m_bucket_states[index].is_occupied()) {
                T const& element = m_buckets[index];
                if (match_predicate(element) == MatchResult::Yes) {
                    m_buckets[index].~T();
                    m_bucket_states[index] = BucketState::tombstone();
                    --m_count;
                }
            }
        }
    }

private:
    NODISCARD ALWAYS_INLINE static constexpr usize calculate_bucket_count(usize required_capacity)
    {
        return (required_capacity * max_load_factor_denominator / max_load_factor_numerator) + 1;
    }

    ALWAYS_INLINE static constexpr void allocate_memory(
        usize in_bucket_count,
        T*& out_buckets,
        BucketState*& out_bucket_states)
    {
        if (in_bucket_count == 0) {
            out_buckets = nullptr;
            out_bucket_states = nullptr;
            return;
        }

        usize allocation_size = in_bucket_count * (sizeof(T) + sizeof(BucketState));
        void* memory = ::operator new(allocation_size);
        out_buckets = static_cast<T*>(memory);
        out_bucket_states = reinterpret_cast<BucketState*>(out_buckets + in_bucket_count);
    }

    ALWAYS_INLINE static constexpr void free_memory(T* in_buckets, usize in_bucket_count)
    {
        MAYBE_UNUSED usize allocation_size = in_bucket_count * (sizeof(T) + sizeof(BucketState));
        ::operator delete(in_buckets);
    }

    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each_internal(Predicate predicate)
    {
        for (usize index = 0; index < m_bucket_count; ++index) {
            if (m_bucket_states[index].is_occupied()) {
                T& element = m_buckets[index];
                if (predicate(element) == IterationDecision::Break)
                    return IterationDecision::Break;
            }
        }
        return IterationDecision::Continue;
    }

    ALWAYS_INLINE void block_reentrant_calls() const
    {
        ASSERTF(m_block_reentrant_calls == false, "A re-entrant call in the HashSet container was detected!");
        m_block_reentrant_calls = true;
    }

    ALWAYS_INLINE void unblock_reentrant_calls() const
    {
        ASSERT(m_block_reentrant_calls == true);
        m_block_reentrant_calls = false;
    }

private:
    usize m_count;
    usize m_bucket_count;
    T* m_buckets;
    BucketState* m_bucket_states;

    // Reentrancy guard for critical sections that mutate bucket state/lifetimes.
    // While true, public APIs assert if called recursively on the same instance
    // (for example, from element constructors/destructors or predicate callbacks).
    // This protects internal invariants; it is not a thread-safety primitive.
    mutable bool m_block_reentrant_calls { false };
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::HashSet;
using AND::HashSetAdd;
using AND::HashSetRemove;
#endif
