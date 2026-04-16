/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

#if !AND_PLATFORM_ENDIANESS_UNKNOWN
NODISCARD ALWAYS_INLINE constexpr bool host_is_little_endian() noexcept
#else
NODISCARD ALWAYS_INLINE bool host_is_little_endian() noexcept
#endif
{
#if AND_PLATFORM_ENDIANESS_LITTLE
    return true;
#elif AND_PLATFORM_ENDIANESS_BIG
    return false;
#else
    static constexpr u8 byte_sequence[] = { 0x00, 0x01 };
    u16 const* word = reinterpret_cast<u16 const*>(byte_sequence);
    return (*word == 0x0100);
#endif
}

#if !AND_PLATFORM_ENDIANESS_UNKNOWN
NODISCARD ALWAYS_INLINE constexpr bool host_is_big_endian() noexcept
#else
NODISCARD ALWAYS_INLINE bool host_is_big_endian() noexcept
#endif
{
    return !host_is_little_endian();
}

NODISCARD ALWAYS_INLINE constexpr u16 reverse_byte_order(u16 value)
{
    constexpr u16 mask_byte_1 = 0x00FF;
    constexpr u16 mask_byte_2 = 0xFF00;

    // clang-format off
    return
        ((value >> 8) & mask_byte_1) |
        ((value << 8) & mask_byte_2);
    // clang-format on
}

NODISCARD ALWAYS_INLINE constexpr u32 reverse_byte_order(u32 value)
{
    constexpr u32 mask_byte_1 = 0x000000FF;
    constexpr u32 mask_byte_2 = 0x0000FF00;
    constexpr u32 mask_byte_3 = 0x00FF0000;
    constexpr u32 mask_byte_4 = 0xFF000000;

    // clang-format off
    return
        ((value >> 24) & mask_byte_1) |
        ((value >> 8)  & mask_byte_2) |
        ((value << 8)  & mask_byte_3) |
        ((value << 24) & mask_byte_4);
    // clang-format on
}

NODISCARD ALWAYS_INLINE constexpr u64 reverse_byte_order(u64 value)
{
    constexpr u64 mask_byte_1 = 0x00000000000000FF;
    constexpr u64 mask_byte_2 = 0x000000000000FF00;
    constexpr u64 mask_byte_3 = 0x0000000000FF0000;
    constexpr u64 mask_byte_4 = 0x00000000FF000000;
    constexpr u64 mask_byte_5 = 0x000000FF00000000;
    constexpr u64 mask_byte_6 = 0x0000FF0000000000;
    constexpr u64 mask_byte_7 = 0x00FF000000000000;
    constexpr u64 mask_byte_8 = 0xFF00000000000000;

    // clang-format off
    return
        ((value >> 56) & mask_byte_1) |
        ((value >> 40) & mask_byte_2) |
        ((value >> 24) & mask_byte_3) |
        ((value >> 8)  & mask_byte_4) |
        ((value << 8)  & mask_byte_5) |
        ((value << 24) & mask_byte_6) |
        ((value << 40) & mask_byte_7) |
        ((value << 56) & mask_byte_8);
    // clang-format on
}

NODISCARD ALWAYS_INLINE constexpr s16 reverse_byte_order(s16 value) { return static_cast<s16>(reverse_byte_order(static_cast<u16>(value))); }
NODISCARD ALWAYS_INLINE constexpr s32 reverse_byte_order(s32 value) { return static_cast<s32>(reverse_byte_order(static_cast<u32>(value))); }
NODISCARD ALWAYS_INLINE constexpr s64 reverse_byte_order(s64 value) { return static_cast<s64>(reverse_byte_order(static_cast<u64>(value))); }

#define AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS(type)                     \
    NODISCARD ALWAYS_INLINE type host_to_little(type host_value)          \
    {                                                                     \
        if (host_is_little_endian())                                      \
            return host_value;                                            \
        return reverse_byte_order(host_value);                            \
    }                                                                     \
                                                                          \
    NODISCARD ALWAYS_INLINE type host_to_big(type host_value)             \
    {                                                                     \
        if (host_is_big_endian())                                         \
            return host_value;                                            \
        return reverse_byte_order(host_value);                            \
    }                                                                     \
                                                                          \
    NODISCARD ALWAYS_INLINE type little_to_host(type little_endian_value) \
    {                                                                     \
        if (host_is_little_endian())                                      \
            return little_endian_value;                                   \
        return reverse_byte_order(little_endian_value);                   \
    }                                                                     \
                                                                          \
    NODISCARD ALWAYS_INLINE type big_to_host(type big_endian_value)       \
    {                                                                     \
        if (host_is_big_endian())                                         \
            return big_endian_value;                                      \
        return reverse_byte_order(big_endian_value);                      \
    }

AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS(u8);
AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS(u16);
AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS(u32);
AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS(u64);

AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS(s8);
AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS(s16);
AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS(s32);
AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS(s64);

#undef AND_DECLARE_ENDIAN_CONVERSION_FUNCTIONS

template<typename T>
requires(is_integer<T>)
class LittleEndian {
public:
    /*implicit*/ ALWAYS_INLINE LittleEndian(T host_value)
        : m_little_endian_value(host_to_little(host_value))
    {
    }

    NODISCARD ALWAYS_INLINE static LittleEndian deserialize(void const* data)
    {
        return LittleEndian(*static_cast<T const*>(data));
    }

    LittleEndian(LittleEndian const&) = default;
    LittleEndian& operator=(LittleEndian const&) = default;

    NODISCARD ALWAYS_INLINE T value() const { return little_to_host(m_little_endian_value); }
    NODISCARD ALWAYS_INLINE T operator*() const { return value(); }
    /*implicit*/ NODISCARD ALWAYS_INLINE operator T() const { return value(); }

    ALWAYS_INLINE void serialize(void* destination) const
    {
        *static_cast<T*>(destination) = m_little_endian_value;
    }

public:
    NODISCARD ALWAYS_INLINE bool operator==(LittleEndian const& other) const { return m_little_endian_value == other.m_little_endian_value; }
    NODISCARD ALWAYS_INLINE bool operator!=(LittleEndian const& other) const { return m_little_endian_value != other.m_little_endian_value; }

private:
    T m_little_endian_value;
};

template<typename T>
requires(is_integer<T>)
class BigEndian {
public:
    /*implicit*/ ALWAYS_INLINE BigEndian(T host_value)
        : m_big_endian_value(host_to_big(host_value))
    {
    }

    NODISCARD ALWAYS_INLINE static BigEndian deserialize(void const* data)
    {
        return BigEndian(*static_cast<T const*>(data));
    }

    BigEndian(BigEndian const&) = default;
    BigEndian& operator=(BigEndian const&) = default;

    NODISCARD ALWAYS_INLINE T value() const { return big_to_host(m_big_endian_value); }
    NODISCARD ALWAYS_INLINE T operator*() const { return value(); }
    /*implicit*/ NODISCARD ALWAYS_INLINE operator T() const { return value(); }

    ALWAYS_INLINE void serialize(void* destination) const
    {
        *static_cast<T*>(destination) = m_big_endian_value;
    }

public:
    NODISCARD ALWAYS_INLINE bool operator==(BigEndian const& other) const { return m_big_endian_value == other.m_big_endian_value; }
    NODISCARD ALWAYS_INLINE bool operator!=(BigEndian const& other) const { return m_big_endian_value != other.m_big_endian_value; }

private:
    T m_big_endian_value;
};

} // namespace AND
