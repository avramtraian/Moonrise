/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Assertion.h"
#include "AT/CoreDefines.h"
#include "AT/CoreTypes.h"
#include "AT/Error.h"
#include "AT/Optional.h"
#include "AT/TypeTraits.h"

namespace AT {

namespace Detail {

template<typename T, typename MetadataType, u8 metadata_available_bit_mask>
class HashTableIterator {
public:
    ALWAYS_INLINE HashTableIterator(T* first_slot, T* last_slot, MetadataType* first_slot_metadata)
        : m_current_slot(first_slot)
        , m_last_slot(last_slot)
        , m_slot_metadata(first_slot_metadata)
    {
        if (m_current_slot != m_last_slot && (*m_slot_metadata & metadata_available_bit_mask)) {
            // NOTE: An iterator has to always be invalidated (m_current_slot == m_last_slot) or point
            //       towards a slot that is occupied.
            skip_to_next();
        }
    }

    NODISCARD ALWAYS_INLINE bool operator==(const HashTableIterator& other) const
    {
        return m_current_slot == other.m_current_slot;
    }
    NODISCARD ALWAYS_INLINE bool operator!=(const HashTableIterator& other) const
    {
        return m_current_slot != other.m_current_slot;
    }

    NODISCARD ALWAYS_INLINE T& operator*() { return *m_current_slot; }
    NODISCARD ALWAYS_INLINE T* operator->() { return m_current_slot; }

    ALWAYS_INLINE HashTableIterator& operator++()
    {
        skip_to_next();
        return *this;
    }

    ALWAYS_INLINE HashTableIterator operator++(int)
    {
        HashTableIterator current = *this;
        ++(*this);
        return current;
    }

private:
    ALWAYS_INLINE void skip_to_next()
    {
        if (m_current_slot == m_last_slot) {
            return;
        }

        do {
            ++m_current_slot;
            ++m_slot_metadata;

            if (m_current_slot == m_last_slot) {
                return;
            }
        } while (*m_slot_metadata & metadata_available_bit_mask);
    }

private:
    T* m_current_slot;
    T* m_last_slot;
    MetadataType* m_slot_metadata;
};

} // namespace Detail

enum class HashTableAddResult {
    InsertedNewEntry,
    EntryAlreadyExists,
};

enum class HashTableRemoveResult {
    RemovedExistingEntry,
    EntryDoesNotExist,
};

template<typename T, typename TraitsForT = TypeTraits<RemoveConst<T>>>
requires (!is_reference<T>)
class HashTable {
    template<typename KeyType, typename ValueType>
    requires (!is_reference<KeyType>)
    friend class HashMap;

public:
    using Metadata = u8;
    static constexpr u8 metadata_empty_value = 0b10000000;
    static constexpr u8 metadata_tombstone_value = 0b11000000;
    static constexpr u8 metadata_available_bit_mask = 0b10000000;
    static constexpr u8 metadata_low_hash_mask = 0b01111111;

    static constexpr usize max_load_factor_percentage = 75;

    ALWAYS_INLINE static u64 get_element_hash(const T& value) { return TraitsForT::hash(value); }
    ALWAYS_INLINE static constexpr u8 get_low_hash(u64 hash_value) { return (hash_value & metadata_low_hash_mask); }
    ALWAYS_INLINE static constexpr u64 get_high_hash(u64 hash_value) { return (hash_value >> 7); }

    using Iterator = Detail::HashTableIterator<const T, const Metadata, metadata_available_bit_mask>;

public:
    ALWAYS_INLINE static ErrorOr<HashTable> create(const HashTable& other)
    {
        if (other.m_occupied_slot_count == 0) {
            return HashTable();
        }

        TRY_ASSIGN(HashTable self, create_with_initial_capacity(other.m_slot_count));
        self.m_occupied_slot_count = other.m_slot_count;

        for (usize index = 0; index < other.m_slot_count; ++index) {
            if (!(other.m_slots_metadata[index] & metadata_available_bit_mask)) {
                const T& element = other.m_slots[index];
                const u64 element_hash = get_element_hash(element);

                const usize slot_index = self.unchecked_find_first_available_slot(element_hash);
                new (self.m_slots + slot_index) T(element);
                self.m_slots_metadata[slot_index] = other.m_slots_metadata[index];
            }
        }

        return self;
    }

    ALWAYS_INLINE static ErrorOr<HashTable> create_with_initial_capacity(usize capacity)
    {
        if (capacity == 0) {
            return HashTable();
        }

        HashTable self;
        self.m_slot_count = calculate_minimal_slot_count(capacity);
        self.m_occupied_slot_count = 0;
        TRY(allocate_and_initialize_memory(self.m_slot_count, self.m_slots, self.m_slots_metadata));

        return self;
    }

    //
    // Creates a hash table that contains the elements held in the given list.
    // If an element exists in the list more than once, only its first occurrence will be inserted.
    //
    ALWAYS_INLINE static ErrorOr<HashTable> create_from_list(std::initializer_list<T> list)
    {
        if (list.size() == 0) {
            return HashTable();
        }

        TRY_ASSIGN(HashTable self, create_with_initial_capacity(list.size()));
        for (usize index = 0; index < list.size(); ++index) {
            TRY(self.add_if_not_existing(list.begin()[index]));
        }

        return self;
    }

    ALWAYS_INLINE static ErrorOr<void> assign(HashTable& self, const HashTable& other)
    {
        const usize minimal_slot_count = calculate_minimal_slot_count(other.m_occupied_slot_count);
        if (minimal_slot_count > self.m_slot_count) {
            TRY(self.clear_and_shrink());
            self.m_slot_count = minimal_slot_count;
            TRY(allocate_and_initialize_memory(self.m_slot_count, self.m_slots, self.m_slots_metadata));
        }
        else {
            self.clear();
        }

        if (other.m_occupied_slot_count == 0) {
            // No more action is required.
            return {};
        }

        self.m_occupied_slot_count = other.m_occupied_slot_count;
        for (usize index = 0; index < other.m_slot_count; ++index) {
            if (!(other.m_slots_metadata[index] & metadata_available_bit_mask)) {
                const T& element = other.m_slots[index];
                const u64 element_hash = get_element_hash(element);

                const usize slot_index = self.unchecked_find_first_available_slot(element_hash);
                new (self.m_slots + slot_index) T(element);
                self.m_slots_metadata[slot_index] = other.m_slots_metadata[index];
            }
        }

        return {};
    }

public:
    ALWAYS_INLINE HashTable()
        : m_slots(nullptr)
        , m_slots_metadata(nullptr)
        , m_slot_count(0)
        , m_occupied_slot_count(0)
    {}

    ALWAYS_INLINE HashTable(const HashTable& other)
    {
        TRY_ASSIGN(HashTable self, create(other));
        new (this) HashTable(move(self));
    }

    ALWAYS_INLINE HashTable(HashTable&& other) noexcept
        : m_slots(other.m_slots)
        , m_slots_metadata(other.m_slots_metadata)
        , m_slot_count(other.m_slot_count)
        , m_occupied_slot_count(other.m_occupied_slot_count)
    {
        other.m_slots = nullptr;
        other.m_slots_metadata = nullptr;
        other.m_slot_count = 0;
        other.m_occupied_slot_count = 0;
    }

    ALWAYS_INLINE ~HashTable() { MUST(clear_and_shrink()); }

    ALWAYS_INLINE HashTable& operator=(const HashTable& other)
    {
        MUST(assign(*this, other));
        return *this;
    }

    ALWAYS_INLINE HashTable& operator=(HashTable&& other) noexcept
    {
        MUST(clear_and_shrink());

        m_slots = other.m_slots;
        m_slots_metadata = other.m_slots_metadata;
        m_slot_count = other.m_slot_count;
        m_occupied_slot_count = other.m_occupied_slot_count;

        other.m_slots = nullptr;
        other.m_slots_metadata = nullptr;
        other.m_slot_count = 0;
        other.m_occupied_slot_count = 0;

        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE Optional<usize> find(const T& element) const
    {
        if (m_occupied_slot_count == 0) {
            // No slots are occupied so the table contains no elements.
            return false;
        }

        const u64 element_hash = get_element_hash(element);
        const u8 low_hash = get_low_hash(element_hash);

        usize index = get_high_hash(element_hash) % m_slot_count;
        for (usize counter = 0; counter < m_slot_count; ++counter) {
            if (m_slots_metadata[index] == low_hash && m_slots[index] == element) {
                // The element has been found.
                return index;
            }
            if (m_slots_metadata[index] == metadata_empty_value) {
                // If we encounter a slot that has never been occupied we can be sure the table
                // doesn't contain the element.
                return {};
            }

            index = (index + 1) % m_slot_count;
        }

        // We checked all occupied slots in the table and found no matches.
        return {};
    }

    NODISCARD ALWAYS_INLINE bool contains(const T& element) const
    {
        const auto index = find(element);
        return index.has_value();
    }

public:
    ALWAYS_INLINE ErrorOr<void> add(const T& element)
    {
        TRY(re_allocate_if_overloaded(m_occupied_slot_count + 1));

        const u64 element_hash = get_element_hash(element);
        const u8 low_hash = get_low_hash(element_hash);
        const usize slot_index = unchecked_find_element_or_first_available_slot(element, element_hash, low_hash);

        if (m_slots_metadata[slot_index] == low_hash) {
            return Error::KeyAlreadyExists;
        }

        new (m_slots + slot_index) T(element);
        m_slots_metadata[slot_index] = low_hash;
        ++m_occupied_slot_count;

        return {};
    }

    ALWAYS_INLINE ErrorOr<void> add(T&& element)
    {
        TRY(re_allocate_if_overloaded(m_occupied_slot_count + 1));

        const u64 element_hash = get_element_hash(element);
        const u8 low_hash = get_low_hash(element_hash);
        const usize slot_index = unchecked_find_element_or_first_available_slot(element, element_hash, low_hash);

        if (m_slots_metadata[slot_index] == low_hash) {
            return Error::KeyAlreadyExists;
        }

        new (m_slots + slot_index) T(move(element));
        m_slots_metadata[slot_index] = low_hash;
        ++m_occupied_slot_count;

        return {};
    }

    ALWAYS_INLINE ErrorOr<HashTableAddResult> add_if_not_existing(const T& element)
    {
        const u64 element_hash = get_element_hash(element);
        const u8 low_hash = get_low_hash(element_hash);

        usize slot_index = unchecked_find_element_or_first_available_slot(element, element_hash, low_hash);
        if (m_slots_metadata[slot_index] == low_hash) {
            // The element already exists in the table.
            return HashTableAddResult::EntryAlreadyExists;
        }

        TRY_ASSIGN(bool has_re_allocated, re_allocate_if_overloaded(m_occupied_slot_count + 1));
        if (has_re_allocated) {
            // NOTE: We already know that the element doesn't exist in the table.
            slot_index = unchecked_find_first_available_slot(element_hash);
        }

        new (m_slots + slot_index) T(element);
        m_slots_metadata[slot_index] = low_hash;
        ++m_occupied_slot_count;

        return HashTableAddResult::InsertedNewEntry;
    }

    ALWAYS_INLINE ErrorOr<HashTableAddResult> add_if_not_existing(T&& element)
    {
        const u64 element_hash = get_element_hash(element);
        const u8 low_hash = get_low_hash(element_hash);

        usize slot_index = unchecked_find_element_or_first_available_slot(element, element_hash, low_hash);
        if (m_slots_metadata[slot_index] == low_hash) {
            // The element already exists in the table.
            return HashTableAddResult::EntryAlreadyExists;
        }

        TRY_ASSIGN(bool has_re_allocated, re_allocate_if_overloaded(m_occupied_slot_count + 1));
        if (has_re_allocated) {
            // NOTE: We already know that the element doesn't exist in the table.
            slot_index = unchecked_find_first_available_slot(element_hash);
        }

        new (m_slots + slot_index) T(move(element));
        m_slots_metadata[slot_index] = low_hash;
        ++m_occupied_slot_count;

        return HashTableAddResult::InsertedNewEntry;
    }

public:
    ALWAYS_INLINE void clear()
    {
        if (m_occupied_slot_count == 0) {
            set_memory(m_slots_metadata, metadata_empty_value, m_slot_count * sizeof(Metadata));
            return;
        }

        for (usize index = 0; index < m_slot_count; ++index) {
            if (!(m_slots_metadata[index] & metadata_available_bit_mask)) {
                m_slots[index].~T();
            }

            m_slots_metadata[index] = metadata_empty_value;
        }
    }

    ALWAYS_INLINE ErrorOr<void> clear_and_shrink()
    {
        if (m_occupied_slot_count > 0) {
            for (usize index = 0; index < m_slot_count; ++index) {
                if (!(m_slots_metadata[index] & metadata_available_bit_mask)) {
                    m_slots[index].~T();
                }
            }
        }

        TRY(release_memory(m_slots, m_slot_count));
        m_slots = nullptr;
        m_slots_metadata = nullptr;
        m_slot_count = 0;
        m_occupied_slot_count = 0;

        return {};
    }

    ALWAYS_INLINE ErrorOr<void> remove(const T& element)
    {
        const auto optional_slot_index = find(element);
        if (!optional_slot_index) {
            return Error::KeyDoesNotExist;
        }

        const usize slot_index = *optional_slot_index;
        m_slots[slot_index].~T();
        m_slots_metadata[slot_index] = metadata_tombstone_value;
        --m_occupied_slot_count;

        return {};
    }

    ALWAYS_INLINE HashTableRemoveResult remove_if_exists(const T& element)
    {
        const auto optional_slot_index = find(element);
        if (!optional_slot_index) {
            return HashTableRemoveResult::EntryDoesNotExist;
        }

        const usize slot_index = *optional_slot_index;
        m_slots[slot_index].~T();
        m_slots_metadata[slot_index] = metadata_tombstone_value;
        --m_occupied_slot_count;

        return HashTableRemoveResult::RemovedExistingEntry;
    }

public:
    NODISCARD ALWAYS_INLINE Iterator begin() const
    {
        return Iterator(m_slots, m_slots + m_slot_count, m_slots_metadata);
    }

    NODISCARD ALWAYS_INLINE Iterator end() const
    {
        return Iterator(m_slots + m_slot_count, m_slots + m_slot_count, m_slots_metadata + m_slot_count);
    }

private:
    ALWAYS_INLINE static ErrorOr<void>
    allocate_and_initialize_memory(usize slot_count, T*& out_slots, Metadata*& out_slots_metadata)
    {
        void* memory_block = ::operator new(slot_count * (sizeof(T) + sizeof(Metadata)));
        if (!memory_block) {
            return Error::OutOfMemory;
        }

        out_slots = reinterpret_cast<T*>(memory_block);
        out_slots_metadata = reinterpret_cast<Metadata*>(out_slots + slot_count);
        set_memory(out_slots_metadata, metadata_empty_value, slot_count * sizeof(Metadata));
        return {};
    }

    ALWAYS_INLINE static ErrorOr<void> release_memory(T* slots, usize slot_count)
    {
        // NOTE: The standard operator delete doesn't need the size of the memory block.
        //       However, in future implementations we might switch to a custom memory allocator,
        //       so having this crucial information available out-of-the-box is really handy.
        MAYBE_UNUSED const usize byte_count = slot_count * (sizeof(T) + sizeof(Metadata));

        ::operator delete(slots);
        return {};
    }

    NODISCARD ALWAYS_INLINE static usize calculate_minimal_slot_count(usize required_count)
    {
        return (required_count * 100 / max_load_factor_percentage) + 1;
    }

    NODISCARD ALWAYS_INLINE static usize calculate_next_slot_count(usize current_slot_count, usize required_slot_count)
    {
        const usize next_slot_count = 2 * current_slot_count;
        if (next_slot_count < required_slot_count) {
            return required_slot_count;
        }
        return next_slot_count;
    }

private:
    NODISCARD ALWAYS_INLINE usize unchecked_find_first_available_slot(u64 element_hash) const
    {
        usize index = get_high_hash(element_hash) % m_slot_count;
        while (true) {
            if (m_slots_metadata[index] & metadata_available_bit_mask) {
                return index;
            }
            index = (index + 1) % m_slot_count;
        }
    }

    // NOTE: Will return 'invalid_size' if no available slot was found and the element doesn't already exist.
    NODISCARD ALWAYS_INLINE usize
    unchecked_find_element_or_first_available_slot(const T& element, u64 element_hash, u8 low_hash) const
    {
        usize index = get_high_hash(element_hash) % m_slot_count;
        usize first_available_slot_index = invalid_size;

        for (usize counter = 0; counter < m_slot_count; ++counter) {
            if (m_slots_metadata[index] == metadata_empty_value) {
                if (first_available_slot_index == invalid_size) {
                    first_available_slot_index = index;
                }
                return first_available_slot_index;
            }
            if (m_slots_metadata[index] == metadata_tombstone_value) {
                if (first_available_slot_index == invalid_size) {
                    first_available_slot_index = index;
                }
            }
            else if (m_slots_metadata[index] == low_hash && m_slots[index] == element) {
                return index;
            }

            index = (index + 1) % m_slot_count;
        }

        AT_ASSERT(first_available_slot_index != invalid_size);
        return first_available_slot_index;
    }

    ALWAYS_INLINE ErrorOr<void> re_allocate_to_fixed(usize new_slot_count)
    {
        AT_ASSERT(new_slot_count >= calculate_minimal_slot_count(m_slot_count));

        T* slots = m_slots;
        Metadata* slots_metadata = m_slots_metadata;
        const usize slot_count = m_slot_count;

        m_slot_count = new_slot_count;
        TRY(allocate_and_initialize_memory(m_slot_count, m_slots, m_slots_metadata));

        for (usize index = 0; index < slot_count; ++index) {
            if (!(slots_metadata[index] & metadata_available_bit_mask)) {
                T& element = slots[index];
                const Metadata slot_metadata = slots_metadata[index];
                const u64 element_hash = get_element_hash(element);

                const usize slot_index = unchecked_find_first_available_slot(element_hash);
                new (m_slots + slot_index) T(move(element));
                m_slots_metadata[slot_index] = slot_metadata;

                element.~T();
            }
        }

        TRY(release_memory(slots, slot_count));
        return {};
    }

    ALWAYS_INLINE ErrorOr<bool> re_allocate_if_overloaded(usize required_count)
    {
        const usize minimal_slot_count = calculate_minimal_slot_count(required_count);
        if (minimal_slot_count > m_slot_count) {
            const usize new_slot_count = calculate_next_slot_count(m_slot_count, minimal_slot_count);
            TRY(re_allocate_to_fixed(new_slot_count));
            return true;
        }

        return false;
    }

private:
    T* m_slots;
    Metadata* m_slots_metadata;
    usize m_slot_count;
    usize m_occupied_slot_count;
};

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::HashTable;
using AT::HashTableAddResult;
using AT::HashTableRemoveResult;
#endif // AT_INCLUDE_GLOBALLY