/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Assertions.h"
#include "AND/Noncopyable.h"
#include "AND/NumericLimits.h"
#include "AND/Types.h"

namespace AND {

class RefCounted {
    AND_MAKE_NONCOPYABLE(RefCounted);
    AND_MAKE_NONMOVABLE(RefCounted);

public:
    RefCounted() = default;
    virtual ~RefCounted() = default;

    ALWAYS_INLINE void increment_ref_count() const
    {
        ASSERT(m_ref_count < AND::NumericLimits<u32>::max());
        ++m_ref_count;
    }

    ALWAYS_INLINE bool decrement_ref_count() const
    {
        ASSERT(m_ref_count > 0);
        --m_ref_count;
        return (m_ref_count == 0);
    }

private:
    mutable u32 m_ref_count { 0 };
};

} // namespace AND
