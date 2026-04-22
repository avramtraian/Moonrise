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

protected:
    RefCounted() = default;

    mutable u32 m_ref_count { 0 };
};

class WeakRefCounted : public RefCounted {
public:
    bool has_weak_ref_count() const
    {
        return (m_weak_ref_count > 0);
    }

    bool has_strong_ref_count() const
    {
        return (m_ref_count > 0);
    }

    u32 weak_ref_count() const
    {
        return m_weak_ref_count;
    }

    void
    increment_weak_ref_count() const
    {
        ASSERT(m_weak_ref_count < AND::NumericLimits<u32>::max());
        ++m_weak_ref_count;
    }

    bool decrement_weak_ref_count() const
    {
        ASSERT(m_weak_ref_count > 0);
        --m_weak_ref_count;
        return (m_weak_ref_count == 0);
    }

public:
    WeakRefCounted() = default;

    mutable u32 m_weak_ref_count { 0 };
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::RefCounted;
using AND::WeakRefCounted;
#endif
