/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Platform.h>
#include <AND/Time.h>

namespace AND {

TimeDuration TimeDuration::from_raw_ticks(u64 ticks)
{
    return TimeDuration { ticks };
}

TimeDuration TimeDuration::from_seconds(f32 seconds)
{
    f64 ticks = static_cast<f64>(seconds) * static_cast<f64>(Platform::ticks_per_second());
    return TimeDuration(static_cast<u64>(ticks));
}

TimeDuration TimeDuration::from_milliseconds(f32 milliseconds)
{
    f32 seconds = milliseconds / 1e3F;
    return from_seconds(seconds);
}

TimeDuration TimeDuration::from_microseconds(f64 microseconds)
{
    f64 seconds = microseconds / 1e6;
    return from_seconds(static_cast<f32>(seconds));
}

TimeDuration TimeDuration::from_nanoseconds(f64 nanoseconds)
{
    f64 seconds = nanoseconds / 1e9;
    return from_seconds(static_cast<f32>(seconds));
}

f32 TimeDuration::seconds() const
{
    f64 ticks = static_cast<f64>(m_ticks);
    f64 ticks_per_second = static_cast<f64>(Platform::ticks_per_second());
    return static_cast<f32>(ticks / ticks_per_second);
}

f32 TimeDuration::milliseconds() const
{
    f64 ticks = static_cast<f64>(m_ticks);
    f64 ticks_per_millisecond = static_cast<f64>(Platform::ticks_per_second()) / 1e3;
    return static_cast<f32>(ticks / ticks_per_millisecond);
}

f64 TimeDuration::microseconds() const
{
    f64 ticks = 1e3 * static_cast<f64>(m_ticks);
    f64 ticks_per_microsecond = static_cast<f64>(Platform::ticks_per_second()) / 1e6;
    return ticks / ticks_per_microsecond;
}

f64 TimeDuration::nanoseconds() const
{
    f64 ticks = 1e3 * static_cast<f64>(m_ticks);
    f64 ticks_per_nanosecond = static_cast<f64>(Platform::ticks_per_second()) / 1e9;
    return ticks / ticks_per_nanosecond;
}

TimePoint TimePoint::now()
{
    u64 current_ticks = Platform::current_time_in_ticks();
    return TimePoint { current_ticks };
}

TimeDuration TimePoint::delta(TimePoint start, TimePoint end)
{
    if (start.m_system_ticks <= end.m_system_ticks) {
        u64 delta_ticks = end.m_system_ticks - start.m_system_ticks;
        return TimeDuration::from_raw_ticks(delta_ticks);
    }
    return TimePoint::delta(end, start);
}

} // namespace AND
