/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Assertions.h"
#include "AND/OpenAddressingBucketState.h"

namespace AND {

template<typename BucketType>
requires(!is_reference<BucketType>)
class OpenAddressingIterator {
public:
    ALWAYS_INLINE OpenAddressingIterator(BucketType* first_bucket, BucketType* end_bucket, OpenAddressingBucketState const* bucket_state)
        : m_current_bucket(first_bucket)
        , m_end_bucket(end_bucket)
        , m_bucket_state(bucket_state)
    {
        while (m_current_bucket != m_end_bucket && !m_bucket_state->is_occupied()) {
            ++m_current_bucket;
            ++m_bucket_state;
        }
    }

    NODISCARD ALWAYS_INLINE BucketType& operator*() const
    {
        ASSERTF(m_current_bucket != m_end_bucket, "Trying to dereference an invalid HashTableIterator!");
        return *m_current_bucket;
    }

    NODISCARD ALWAYS_INLINE BucketType* operator->() const
    {
        ASSERTF(m_current_bucket != m_end_bucket, "Trying to dereference an invalid HashTableIterator!");
        return m_current_bucket;
    }

    ALWAYS_INLINE OpenAddressingIterator& operator++()
    {
        ASSERTF(m_current_bucket != m_end_bucket, "Trying to increment an invalid HashTableIterator!");
        do {
            ++m_current_bucket;
            ++m_bucket_state;
        } while (m_current_bucket != m_end_bucket && !m_bucket_state->is_occupied());
        return *this;
    }

    ALWAYS_INLINE OpenAddressingIterator operator++(int)
    {
        OpenAddressingIterator copy = *this;
        this->operator++();
        return copy;
    }

    NODISCARD ALWAYS_INLINE bool operator==(OpenAddressingIterator const& other) const { return m_current_bucket == other.m_current_bucket; }
    NODISCARD ALWAYS_INLINE bool operator!=(OpenAddressingIterator const& other) const { return m_current_bucket != other.m_current_bucket; }

private:
    BucketType* m_current_bucket;
    BucketType* m_end_bucket;
    OpenAddressingBucketState const* m_bucket_state;
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::OpenAddressingIterator;
#endif
