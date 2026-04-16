/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Defines.h"

namespace AND {

using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

using s8 = signed char;
using s16 = signed short;
using s32 = signed int;
using s64 = signed long long;

using b8 = bool;
using b32 = int;
using f32 = float;
using f64 = double;

#if AND_PLATFORM_ARCH_X86 || AND_PLATFORM_ARCH_ARM32
using usize = u32;
using ssize = s32;
using uintptr = u32;
#endif

#if AND_PLATFORM_ARCH_X64 || AND_PLATFORM_ARCH_ARM64
using usize = u64;
using ssize = s64;
using uintptr = u64;
#endif

static_assert(sizeof(u8) == 1, "sizeof(u8) expected to be 1!");
static_assert(sizeof(u16) == 2, "sizeof(u16) expected to be 2!");
static_assert(sizeof(u32) == 4, "sizeof(u32) expected to be 4!");
static_assert(sizeof(u64) == 8, "sizeof(u64) expected to be 8!");

static_assert(sizeof(s8) == 1, "sizeof(s8) expected to be 1!");
static_assert(sizeof(s16) == 2, "sizeof(s16) expected to be 2!");
static_assert(sizeof(s32) == 4, "sizeof(s32) expected to be 4!");
static_assert(sizeof(s64) == 8, "sizeof(s64) expected to be 8!");

static_assert(sizeof(b8) == 1, "sizeof(b8) expected to be 1!");
static_assert(sizeof(b32) == 4, "sizeof(b32) expected to be 4!");
static_assert(sizeof(f32) == 4, "sizeof(f32) expected to be 4!");
static_assert(sizeof(f64) == 8, "sizeof(f64) expected to be 8!");

static_assert(sizeof(usize) == sizeof(void*), "sizeof(usize) expected to be sizeof(void*)!");
static_assert(sizeof(ssize) == sizeof(void*), "sizeof(ssize) expected to be sizeof(void*)!");
static_assert(sizeof(uintptr) == sizeof(void*), "sizeof(uintptr) expected to be sizeof(void*)!");

using ROByte = u8 const;
using WOByte = u8;
using RWByte = u8;

using ROBytes = ROByte*;
using WOBytes = WOByte*;
using RWBytes = RWByte*;

namespace Internal {

template<typename T>
struct ConstTraits {
    static constexpr bool is_const = false;
    using RemoveConstType = T;
};

template<typename T>
struct ConstTraits<T const> {
    static constexpr bool is_const = false;
    using RemoveConstType = T;
};

template<typename T>
struct ReferenceTraits {
    static constexpr bool is_lvalue_reference = false;
    static constexpr bool is_rvalue_reference = false;
    using RemoveReferenceType = T;
};

template<typename T>
struct ReferenceTraits<T&> {
    static constexpr bool is_lvalue_reference = true;
    static constexpr bool is_rvalue_reference = false;
    using RemoveReferenceType = T;
};

template<typename T>
struct ReferenceTraits<T&&> {
    static constexpr bool is_lvalue_reference = false;
    static constexpr bool is_rvalue_reference = true;
    using RemoveReferenceType = T;
};

template<typename T>
struct NumberTraits {
    static constexpr bool is_unsigned_integer = false;
    static constexpr bool is_signed_integer = false;
    static constexpr bool is_floating_point = false;
};

#define AND_DECLARE_NUMBER_TRAITS(type, unsigned_value, signed_value, float_value) \
    template<>                                                                     \
    struct NumberTraits<type> {                                                    \
        static constexpr bool is_unsigned_integer = unsigned_value;                \
        static constexpr bool is_signed_integer = signed_value;                    \
        static constexpr bool is_floating_point = float_value;                     \
    }

AND_DECLARE_NUMBER_TRAITS(unsigned char, true, false, false);
AND_DECLARE_NUMBER_TRAITS(unsigned short, true, false, false);
AND_DECLARE_NUMBER_TRAITS(unsigned int, true, false, false);
AND_DECLARE_NUMBER_TRAITS(unsigned long, true, false, false);
AND_DECLARE_NUMBER_TRAITS(unsigned long long, true, false, false);
AND_DECLARE_NUMBER_TRAITS(signed char, false, true, false);
AND_DECLARE_NUMBER_TRAITS(signed short, false, true, false);
AND_DECLARE_NUMBER_TRAITS(signed int, false, true, false);
AND_DECLARE_NUMBER_TRAITS(signed long, false, true, false);
AND_DECLARE_NUMBER_TRAITS(signed long long, false, true, false);
AND_DECLARE_NUMBER_TRAITS(float, false, false, true);
AND_DECLARE_NUMBER_TRAITS(double, false, false, true);

#undef AND_DECLARE_NUMBER_TRAITS

template<typename T, typename Q>
struct IsSame {
    static constexpr bool value = false;
};

template<typename T>
struct IsSame<T, T> {
    static constexpr bool value = true;
};

template<bool condition, typename TypeIfTrue, typename TypeIfFalse>
struct ConditionalType {
};

template<typename TypeIfTrue, typename TypeIfFalse>
struct ConditionalType<true, TypeIfTrue, TypeIfFalse> {
    using Type = TypeIfTrue;
};

template<typename TypeIfTrue, typename TypeIfFalse>
struct ConditionalType<false, TypeIfTrue, TypeIfFalse> {
    using Type = TypeIfFalse;
};

} // namespace Internal

template<typename T>
static constexpr bool is_const = Internal::ConstTraits<T>::is_const;

template<typename T>
static constexpr bool is_lvalue_reference = Internal::ReferenceTraits<T>::is_lvalue_reference;

template<typename T>
static constexpr bool is_rvalue_reference = Internal::ReferenceTraits<T>::is_rvalue_reference;

template<typename T>
static constexpr bool is_reference = is_lvalue_reference<T> || is_rvalue_reference<T>;

template<typename T>
static constexpr bool is_unsigned_integer = Internal::NumberTraits<T>::is_unsigned_integer;

template<typename T>
static constexpr bool is_signed_integer = Internal::NumberTraits<T>::is_signed_integer;

template<typename T>
static constexpr bool is_floating_point = Internal::NumberTraits<T>::is_floating_point;

template<typename T>
static constexpr bool is_integer = is_unsigned_integer<T> || is_signed_integer<T>;

template<typename T>
static constexpr bool is_number = is_integer<T> || is_floating_point<T>;

template<typename T>
static constexpr bool is_signed = is_signed_integer<T> || is_floating_point<T>;

template<typename T>
static constexpr bool is_unsigned = !is_signed<T>;

template<typename T, typename Q>
static constexpr bool is_same = Internal::IsSame<T, Q>::value;

template<typename BaseType, typename DerivedType>
static constexpr bool is_base_of = __is_base_of(BaseType, DerivedType);

template<typename DerivedType, typename BaseType>
static constexpr bool is_derived_from = is_base_of<BaseType, DerivedType>;

template<typename FromType, typename ToType>
static constexpr bool is_convertible = __is_convertible_to(FromType, ToType);

template<typename T>
using RemoveConst = Internal::ConstTraits<T>::RemoveConstType;

template<typename T>
using RemoveReference = Internal::ReferenceTraits<T>::RemoveReferenceType;

template<bool condition, typename TypeIfTrue, typename TypeIfFalse>
using ConditionalType = Internal::ConditionalType<condition, TypeIfTrue, TypeIfFalse>::Type;

template<typename E>
using UnderlyingType = __underlying_type(E);

template<typename T>
NODISCARD ALWAYS_INLINE constexpr RemoveReference<T>&& move(T&& value) noexcept
{
    return static_cast<RemoveReference<T>&&>(value);
}

template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>& value) noexcept
{
    return static_cast<T&&>(value);
}

template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>&& value) noexcept
{
    return static_cast<T&&>(value);
}

template<typename E>
NODISCARD ALWAYS_INLINE constexpr UnderlyingType<E> to_underlying(E value) noexcept
{
    return static_cast<UnderlyingType<E>>(value);
}

} // namespace AND
