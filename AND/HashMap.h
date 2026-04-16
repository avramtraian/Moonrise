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
#include "AND/OpenAddressingIterator.h"
#include "AND/Optional.h"
#include "AND/Types.h"

#include <initializer_list>
#include <new>

namespace AND {

enum class HashMapAdd : u8 {
    DidAdd,
    ReplacedExisting,
};

enum class HashMapRemove : u8 {
    DidRemove,
    NotFound,
};

template<typename KeyType, typename ValueType>
concept HashMapPair = !is_reference<KeyType> && !is_const<KeyType> && !is_reference<ValueType> && !is_const<ValueType>;

template<typename KeyType, typename ValueType>
requires HashMapPair<KeyType, ValueType>
class HashMapBucket {
public:
    ALWAYS_INLINE explicit constexpr HashMapBucket(KeyType const& key)
        : key(key)
        , value()
    {
    }

    ALWAYS_INLINE explicit constexpr HashMapBucket(KeyType&& key)
        : key(move(key))
        , value()
    {
    }

    ALWAYS_INLINE constexpr HashMapBucket(KeyType const& key, ValueType const& value)
        : key(key)
        , value(value)
    {
    }

    ALWAYS_INLINE constexpr HashMapBucket(KeyType const& key, ValueType&& value)
        : key(key)
        , value(move(value))
    {
    }

    ALWAYS_INLINE constexpr HashMapBucket(KeyType&& key, ValueType const& value)
        : key(move(key))
        , value(value)
    {
    }

    ALWAYS_INLINE constexpr HashMapBucket(KeyType&& key, ValueType&& value)
        : key(move(key))
        , value(move(value))
    {
    }

    ALWAYS_INLINE constexpr HashMapBucket(HashMapBucket const& other)
        : key(other.key)
        , value(other.value)
    {
    }

    ALWAYS_INLINE constexpr HashMapBucket(HashMapBucket&& other) noexcept
        : key(move(other.key))
        , value(move(other.value))
    {
    }

    NODISCARD ALWAYS_INLINE bool operator==(HashMapBucket const& other) const
    {
        bool keys_are_equal = key == other.key;
        return keys_are_equal;
    }

public:
    KeyType const key;
    ValueType value;
};

template<typename KeyType, typename ValueType>
requires HashMapPair<KeyType, ValueType>
class HashMap {
public:
    template<typename OtherKeyType, typename OtherValueType>
    requires HashMapPair<OtherKeyType, OtherValueType>
    friend class HashMap;

    using Bucket = HashMapBucket<KeyType, ValueType>;
    using BucketState = OpenAddressingBucketState;

    using Iterator = OpenAddressingIterator<Bucket>;
    using ConstIterator = OpenAddressingIterator<Bucket const>;

    static constexpr usize max_load_factor_numerator = 3;
    static constexpr usize max_load_factor_denominator = 4;
    static_assert(max_load_factor_numerator <= max_load_factor_denominator && max_load_factor_numerator > 0);

    struct ScopedReentrantBlock {
    public:
        explicit ScopedReentrantBlock(HashMap const& hash_map)
            : m_hash_map(hash_map)
        {
            m_hash_map.block_reentrant_calls();
        }

        ~ScopedReentrantBlock()
        {
            m_hash_map.unblock_reentrant_calls();
        }

    private:
        HashMap& m_hash_map;
    };

#define AND_HASH_MAP_SCOPED_REENTRANT_BLOCK \
    auto AND_CONCAT(scoped, AND_LINE) = ScopedReentrantBlock(*this)

#define AND_HASH_MAP_CHECK_IF_UNBLOCKED \
    ASSERTF(m_block_reentrant_calls == false, "A re-entrant call in the HashMap container was detected!")

public:
    NODISCARD ALWAYS_INLINE static HashMap from_initial_capacity(usize initial_capacity)
    {
        HashMap hash_map;
        hash_map.m_bucket_count = calculate_bucket_count(initial_capacity);
        HashMap::allocate_memory(hash_map.m_bucket_count, hash_map.m_buckets, hash_map.m_bucket_states);
        for (usize index = 0; index < hash_map.m_bucket_count; ++index)
            hash_map.m_bucket_states[index] = BucketState::empty();

        return hash_map;
    }

    NODISCARD ALWAYS_INLINE static HashMap from_init_list(std::initializer_list<Bucket> init_list)
    {
        HashMap hash_map = HashMap::from_initial_capacity(init_list.size());
        for (Bucket const& bucket : init_list)
            hash_map.add(bucket.key, bucket.value);
        return hash_map;
    }

public:
    ALWAYS_INLINE HashMap()
        : m_count(0)
        , m_bucket_count(0)
        , m_buckets(nullptr)
        , m_bucket_states(nullptr)
    {
    }

    ALWAYS_INLINE ~HashMap()
    {
        clear_and_shrink();
    }

    ALWAYS_INLINE HashMap(HashMap const& other)
        : m_count(0)
        , m_bucket_count(0)
        , m_buckets(nullptr)
        , m_bucket_states(nullptr)
    {
        ensure_capacity(other.m_count);
        other.for_each([&](KeyType const& key, ValueType const& value) {
            add(key, value);
            return IterationDecision::Continue;
        });
    }

    ALWAYS_INLINE HashMap(HashMap&& other) noexcept
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

    ALWAYS_INLINE HashMap& operator=(HashMap const& other)
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        clear();
        ensure_capacity(other.m_count);
        other.for_each([&](KeyType const& key, ValueType const& value) {
            add(key, value);
            return IterationDecision::Continue;
        });

        return *this;
    }

    ALWAYS_INLINE HashMap& operator=(HashMap&& other) noexcept
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
    // If the key already exists, the value is overwritten.
    ALWAYS_INLINE HashMapAdd add(KeyType key, ValueType value)
    {
        AND_HASH_MAP_CHECK_IF_UNBLOCKED;
        ensure_capacity(m_count + 1);

        Hash hash = get_hash(key);
        usize base_index = hash % m_bucket_count;

        Optional<usize> destination_index;
        HashMapAdd return_value = HashMapAdd::DidAdd;

        for (usize iteration = 0; iteration < m_bucket_count; ++iteration) {
            usize index = (base_index + iteration) % m_bucket_count;
            BucketState& state = m_bucket_states[index];

            if (state.is_occupied()) {
                if (state.matches_hash(hash) && key == m_buckets[index].key) {
                    AND_HASH_MAP_CHECK_IF_UNBLOCKED;
                    m_buckets[index].~Bucket();
                    state = BucketState::tombstone();
                    --m_count;
                    return_value = HashMapAdd::ReplacedExisting;

                    if (!destination_index.has_value())
                        destination_index = index;
                    break;
                }
            } else {
                if (!destination_index.has_value())
                    destination_index = index;

                if (!state.was_ever_occupied())
                    break;
            }
        }

        {
            AND_HASH_MAP_CHECK_IF_UNBLOCKED;
            ASSERT(destination_index.has_value());
            usize index = destination_index.value();
            new (m_buckets + index) Bucket(move(key), move(value));
            m_bucket_states[index] = BucketState::occupied(hash);
            ++m_count;
        }

        return return_value;
    }

    NODISCARD ALWAYS_INLINE ValueType& operator[](KeyType const& key)
    {
        AND_HASH_MAP_CHECK_IF_UNBLOCKED;
        ensure_capacity(m_count + 1);

        Hash hash = get_hash(key);
        usize base_index = hash % m_bucket_count;
        Optional<usize> destination_index;

        for (usize iteration = 0; iteration < m_bucket_count; ++iteration) {
            usize index = (base_index + iteration) % m_bucket_count;
            auto state = m_bucket_states[index];

            if (state.is_occupied()) {
                if (state.matches_hash(hash) && key == m_buckets[index].key)
                    return m_buckets[index].value;
            } else {
                if (!destination_index.has_value())
                    destination_index = index;

                if (!state.was_ever_occupied())
                    break;
            }
        }

        {
            AND_HASH_MAP_CHECK_IF_UNBLOCKED;
            ASSERT(destination_index.has_value());
            new (m_buckets + *destination_index) Bucket(move(key));
            m_bucket_states[*destination_index] = BucketState::occupied(hash);
            ++m_count;
        }

        return m_buckets[*destination_index].value;
    }

    ALWAYS_INLINE HashMapRemove remove(KeyType const& key)
    {
        AND_HASH_MAP_CHECK_IF_UNBLOCKED;

        // Avoid division (modulo operation) by zero.
        if (m_bucket_count == 0)
            return HashMapRemove::NotFound;

        Hash hash = get_hash(key);
        usize base_index = hash % m_bucket_count;

        for (usize iteration = 0; iteration < m_bucket_count; ++iteration) {
            usize index = (base_index + iteration) % m_bucket_count;
            BucketState& state = m_bucket_states[index];

            if (state.was_ever_occupied()) {
                if (state.is_occupied() && state.matches_hash(hash) && key == m_buckets[index].key) {
                    AND_HASH_MAP_CHECK_IF_UNBLOCKED;
                    m_buckets[index].~Bucket();
                    state = BucketState::tombstone();
                    --m_count;

                    return HashMapRemove::DidRemove;
                }
            } else {
                // We can be certain that the key does not exist in the map.
                return HashMapRemove::NotFound;
            }
        }

        // We iterated over all the buckets and still haven't found the key.
        return HashMapRemove::NotFound;
    }

    NODISCARD ALWAYS_INLINE Optional<ValueType&> get(KeyType const& key)
    {
        Optional<usize> bucket_index = find_bucket_index(key);
        if (!bucket_index.has_value())
            return {};
        return m_buckets[*bucket_index].value;
    }

    NODISCARD ALWAYS_INLINE Optional<ValueType const&> get(KeyType const& key) const
    {
        Optional<usize> bucket_index = find_bucket_index(key);
        if (!bucket_index.has_value())
            return {};
        return m_buckets[*bucket_index].value;
    }

    NODISCARD ALWAYS_INLINE bool contains(KeyType const& key) const
    {
        return find_bucket_index(key).has_value();
    }

public:
    ALWAYS_INLINE void clear()
    {
        AND_HASH_MAP_CHECK_IF_UNBLOCKED;
        for (usize index = 0; index < m_bucket_count; ++index) {
            if (m_bucket_states[index].is_occupied())
                m_buckets[index].~Bucket();
            m_bucket_states[index] = BucketState::empty();
        }
        m_count = 0;
    }

    ALWAYS_INLINE void clear_and_shrink()
    {
        AND_HASH_MAP_CHECK_IF_UNBLOCKED;
        for_each_bucket([&](Bucket& bucket) { bucket.~Bucket(); return IterationDecision::Continue; });
        HashMap::free_memory(m_buckets, m_bucket_count);
        m_count = 0;
        m_bucket_count = 0;
        m_buckets = nullptr;
        m_bucket_states = nullptr;
    }

    ALWAYS_INLINE void ensure_capacity(usize minimum_capacity)
    {
        usize min_bucket_count = calculate_bucket_count(minimum_capacity);
        if (min_bucket_count <= m_bucket_count)
            return;

        HashMap new_hash_map = HashMap::from_initial_capacity(minimum_capacity);
        for_each_bucket([&](Bucket& bucket) {
            new_hash_map.add(move(bucket.key), move(bucket.value));
            return IterationDecision::Continue;
        });
        *this = move(new_hash_map);
    }

public:
    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each(Predicate predicate)
    {
        return for_each_bucket([&](Bucket& bucket) {
            KeyType const& key = bucket.key;
            ValueType& value = bucket.value;
            return predicate(key, value);
        });
    }

    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each(Predicate predicate) const
    {
        return for_each_bucket([&](Bucket const& bucket) {
            KeyType const& key = bucket.key;
            ValueType const& value = bucket.value;
            return predicate(key, value);
        });
    }

    template<typename MatchPredicate>
    ALWAYS_INLINE void remove_all_matching(MatchPredicate match_predicate)
    {
        AND_HASH_MAP_CHECK_IF_UNBLOCKED;

        for (usize index = 0; index < m_bucket_count; ++index) {
            if (m_bucket_states[index].is_occupied()) {
                Bucket& bucket = m_buckets[index];
                KeyType const& key = bucket.key;
                ValueType& value = bucket.value;

                if (match_predicate(key, value) == MatchResult::Yes) {
                    bucket.~Bucket();
                    m_bucket_states[index] = BucketState::tombstone();
                    --m_count;
                }
            }
        }
    }

public:
    NODISCARD ALWAYS_INLINE Iterator begin() { return Iterator(m_buckets, m_buckets + m_bucket_count, m_bucket_states); }
    NODISCARD ALWAYS_INLINE Iterator end() { return Iterator(m_buckets + m_bucket_count, m_buckets + m_bucket_count, m_bucket_states + m_bucket_count); }

    NODISCARD ALWAYS_INLINE ConstIterator begin() const { return ConstIterator(m_buckets, m_buckets + m_bucket_count, m_bucket_states); }
    NODISCARD ALWAYS_INLINE ConstIterator end() const { return ConstIterator(m_buckets + m_bucket_count, m_buckets + m_bucket_count, m_bucket_states + m_bucket_count); }

private:
    NODISCARD ALWAYS_INLINE static constexpr usize calculate_bucket_count(usize required_capacity)
    {
        return (required_capacity * max_load_factor_denominator / max_load_factor_numerator) + 1;
    }

    ALWAYS_INLINE static constexpr void allocate_memory(usize in_bucket_count, Bucket*& out_buckets, BucketState*& out_bucket_states)
    {
        if (in_bucket_count == 0) {
            out_buckets = nullptr;
            out_bucket_states = nullptr;
            return;
        }

        usize allocation_size = in_bucket_count * (sizeof(Bucket) + sizeof(BucketState));
        void* memory = ::operator new(allocation_size);
        out_buckets = static_cast<Bucket*>(memory);
        out_bucket_states = reinterpret_cast<BucketState*>(out_buckets + in_bucket_count);
    }

    ALWAYS_INLINE static constexpr void free_memory(Bucket* in_buckets, usize in_bucket_count)
    {
        MAYBE_UNUSED usize allocation_size = in_bucket_count * (sizeof(Bucket) + sizeof(BucketState));
        ::operator delete(in_buckets);
    }

    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each_bucket(Predicate predicate)
    {
        for (usize index = 0; index < m_bucket_count; ++index) {
            if (m_bucket_states[index].is_occupied()) {
                Bucket& bucket = m_buckets[index];
                if (predicate(bucket) == IterationDecision::Break)
                    return IterationDecision::Break;
            }
        }
        return IterationDecision::Continue;
    }

    template<typename Predicate>
    ALWAYS_INLINE IterationDecision for_each_bucket(Predicate predicate) const
    {
        for (usize index = 0; index < m_bucket_count; ++index) {
            if (m_bucket_states[index].is_occupied()) {
                Bucket const& bucket = m_buckets[index];
                if (predicate(bucket) == IterationDecision::Break)
                    return IterationDecision::Break;
            }
        }
        return IterationDecision::Continue;
    }

    NODISCARD ALWAYS_INLINE Optional<usize> find_bucket_index(KeyType const& key) const
    {
        // Avoid division (modulo operation) by zero.
        if (m_bucket_count == 0)
            return {};

        Hash hash = get_hash(key);
        usize base_index = hash % m_bucket_count;

        for (usize iteration = 0; iteration < m_bucket_count; ++iteration) {
            usize index = (base_index + iteration) % m_bucket_count;
            BucketState state = m_bucket_states[index];

            if (state.was_ever_occupied()) {
                if (state.is_occupied() && state.matches_hash(hash) && key == m_buckets[index].key)
                    return index;
            } else {
                // We can be certain that the key does not exist in the map.
                return {};
            }
        }

        // We iterated over all the buckets and still haven't found the key.
        return {};
    }

    ALWAYS_INLINE void block_reentrant_calls() const
    {
        ASSERTF(m_block_reentrant_calls == false, "A re-entrant call in the HashMap container was detected!");
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
    Bucket* m_buckets;
    BucketState* m_bucket_states;

    // Reentrancy guard for map operations that mutate probing/lifetime state.
    // Enabled during sensitive internals, so recursive calls into this map instance
    // are detected and rejected via assertions.
    // Intended for invariant protection only (not for multi-thread synchronization).
    mutable bool m_block_reentrant_calls { false };
};

} // namespace AND
