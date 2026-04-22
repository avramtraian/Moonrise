/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Types.h>

namespace AND {

class TimeDuration {
public:
    NODISCARD static TimeDuration from_raw_ticks(u64);
    NODISCARD static TimeDuration from_seconds(f32);
    NODISCARD static TimeDuration from_milliseconds(f32);
    NODISCARD static TimeDuration from_microseconds(f64);
    NODISCARD static TimeDuration from_nanoseconds(f64);

public:
    TimeDuration() = default;
    TimeDuration(TimeDuration const&) = default;
    TimeDuration& operator=(TimeDuration const&) = default;

    NODISCARD ALWAYS_INLINE u64 raw_ticks() const { return m_ticks; }
    NODISCARD f32 seconds() const;
    NODISCARD f32 milliseconds() const;
    NODISCARD f64 microseconds() const;
    NODISCARD f64 nanoseconds() const;

private:
    ALWAYS_INLINE constexpr explicit TimeDuration(u64 ticks)
        : m_ticks(ticks)
    {
    }

private:
    u64 m_ticks { 0 };
};

class TimePoint {
public:
    static TimePoint now();
    static TimeDuration delta(TimePoint start, TimePoint end);

public:
    TimePoint() = default;
    TimePoint(TimePoint const&) = default;
    TimePoint& operator=(TimePoint const&) = default;

    NODISCARD ALWAYS_INLINE u64 raw_ticks() const { return m_system_ticks; }
    NODISCARD ALWAYS_INLINE bool is_null() const { return (m_system_ticks == 0); }

private:
    explicit TimePoint(u64 system_ticks)
        : m_system_ticks(system_ticks)
    {
    }

    u64 m_system_ticks { 0 };
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::TimeDuration;
using AND::TimePoint;
#endif
