/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Hash.h"
#include "AND/Types.h"

namespace AND {

struct OpenAddressingBucketState {
public:
    static constexpr u8 is_occupied_bit = 0x1;
    static constexpr u8 was_ever_occupied_bit = 0x2;
    static constexpr u8 hash_mask = 0xFC;

    NODISCARD ALWAYS_INLINE static constexpr OpenAddressingBucketState occupied(Hash hash)
    {
        u8 state = ((hash & 0x3F) << 2) | is_occupied_bit | was_ever_occupied_bit;
        return OpenAddressingBucketState { state };
    }

    NODISCARD ALWAYS_INLINE static constexpr OpenAddressingBucketState tombstone()
    {
        u8 state = was_ever_occupied_bit;
        return OpenAddressingBucketState { state };
    }

    NODISCARD ALWAYS_INLINE static constexpr OpenAddressingBucketState empty()
    {
        u8 state = 0;
        return OpenAddressingBucketState { state };
    }

public:
    NODISCARD ALWAYS_INLINE constexpr bool is_occupied() const { return (state & is_occupied_bit); }
    NODISCARD ALWAYS_INLINE constexpr bool was_ever_occupied() const { return (state & was_ever_occupied_bit); }
    NODISCARD ALWAYS_INLINE constexpr u8 matches_hash(Hash hash) const { return ((state & hash_mask) >> 2) == (hash & 0x3F); }

public:
    u8 state { 0 };
};

} // namespace AND
