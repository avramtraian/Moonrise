/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Defines.h>

// NOTE: std::initializer_list can't be implemented by us, so we include its header file here in
//       order to have access to it throughout the entire codebase.
#include <initializer_list>
#include <type_traits>

namespace AT {

//
// Unsigned integer primitive types with fixed size.
// Guaranteed to be the same size on all platforms/architectures.
//
using u8 = unsigned char;
using u16 = unsigned short;
using u32 = unsigned int;
using u64 = unsigned long long;

//
// Signed integer primitive types with fixed size.
// Guaranteed to be the same size on all platforms/architectures.
//
using i8 = signed char;
using i16 = signed short;
using i32 = signed int;
using i64 = signed long long;

// Checks that ensure the sizes of the primitive integer types are as expected.
// If a mismatch is found, we do not allow the compilation to succeed, as
// unexpected issues related to alignment or integer overflow may occur.
static_assert(sizeof(u8) == 1);
static_assert(sizeof(u16) == 2);
static_assert(sizeof(u32) == 4);
static_assert(sizeof(u64) == 8);
static_assert(sizeof(i8) == 1);
static_assert(sizeof(i16) == 2);
static_assert(sizeof(i32) == 4);
static_assert(sizeof(i64) == 8);

//
// Integers types that can exactly represent a memory address on the current architecture.
// Never assume their sizes, as they can vary depending on the platform and architecture.
//
using usize = unsigned long long;
using ssize = signed long long;
using uintptr = unsigned long long;

constexpr usize invalid_size = static_cast<usize>(-1);

//
// Integer types that represent a byte and have the allowed access modifiers attached to their name.
//
using ReadonlyByte = const u8;
using WriteonlyByte = u8;
using ReadWriteByte = u8;

//
// Types that define a contiguous array of the specified byte types.
//
using ReadonlyBytes = ReadonlyByte*;
using WriteonlyBytes = WriteonlyByte*;
using ReadWriteBytes = ReadWriteByte*;

//
// Unsigned integer that represents the numerical value that maps to a specific character in the Unicode scheme.
// Guaranteed to be the same size (8-bytes) on all platforms/architectures.
//
using UnicodeCodepoint = u64;
constexpr UnicodeCodepoint invalid_unicode_codepoint = static_cast<UnicodeCodepoint>(-1);

#define AT_MAKE_NONCOPYABLE(type_name)    \
    type_name(const type_name&) = delete; \
    type_name& operator=(const type_name&) = delete

#define AT_MAKE_NONMOVABLE(type_name)         \
    type_name(type_name&&) noexcept = delete; \
    type_name& operator=(type_name&&) noexcept = delete

namespace Detail {

template<typename T>
struct RemoveReference {
    using Type = T;
};
template<typename T>
struct RemoveReference<T&> {
    using Type = T;
};
template<typename T>
struct RemoveReference<T&&> {
    using Type = T;
};

template<typename T>
struct RemoveConst {
    using Type = T;
};
template<typename T>
struct RemoveConst<const T> {
    using Type = T;
};

template<typename T>
struct IsReference {
    static constexpr bool value = false;
};
template<typename T>
struct IsReference<T&> {
    static constexpr bool value = true;
};
template<typename T>
struct IsReference<T&&> {
    static constexpr bool value = true;
};

template<typename T>
struct IsConst {
    static constexpr bool value = false;
};
template<typename T>
struct IsConst<const T> {
    static constexpr bool value = true;
};

template<typename T>
struct IsUnsignedIntegral {
    static constexpr bool value = false;
};
template<>
struct IsUnsignedIntegral<u8> {
    static constexpr bool value = true;
};
template<>
struct IsUnsignedIntegral<u16> {
    static constexpr bool value = true;
};
template<>
struct IsUnsignedIntegral<u32> {
    static constexpr bool value = true;
};
template<>
struct IsUnsignedIntegral<u64> {
    static constexpr bool value = true;
};

template<typename T>
struct IsSignedIntegral {
    static constexpr bool value = false;
};
template<>
struct IsSignedIntegral<i8> {
    static constexpr bool value = true;
};
template<>
struct IsSignedIntegral<i16> {
    static constexpr bool value = true;
};
template<>
struct IsSignedIntegral<i32> {
    static constexpr bool value = true;
};
template<>
struct IsSignedIntegral<i64> {
    static constexpr bool value = true;
};

template<typename TypeIfTrue, typename TypeIfFalse, bool condition>
struct ConditionalType {};

template<typename TypeIfTrue, typename TypeIfFalse>
struct ConditionalType<TypeIfTrue, TypeIfFalse, true> {
    using Type = TypeIfTrue;
};

template<typename TypeIfTrue, typename TypeIfFalse>
struct ConditionalType<TypeIfTrue, TypeIfFalse, false> {
    using Type = TypeIfFalse;
};

} // namespace Detail

template<typename T>
using RemoveReference = typename Detail::RemoveReference<T>::Type;
template<typename T>
using RemoveConst = typename Detail::RemoveConst<T>::Type;

template<typename T>
constexpr bool is_reference = Detail::IsReference<T>::value;
template<typename T>
constexpr bool is_const = Detail::IsConst<T>::value;
template<typename T>
constexpr bool is_unsigned_integral = Detail::IsUnsignedIntegral<T>::value;
template<typename T>
constexpr bool is_signed_integral = Detail::IsSignedIntegral<T>::value;
template<typename T>
constexpr bool is_integral = is_unsigned_integral<T> || is_signed_integral<T>;

//
// The STL equivalent of the move function. Same signature and behaviour.
// https://en.cppreference.com/w/cpp/utility/move
//
//
template<typename T>
NODISCARD ALWAYS_INLINE constexpr RemoveReference<T>&& move(T&& instance) noexcept
{
    return static_cast<RemoveReference<T>&&>(instance);
}

//
// The STL equivalent of the forward function. Same signature and behaviour.
// https://en.cppreference.com/w/cpp/utility/forward
//
template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>& instance) noexcept
{
    return static_cast<T&&>(instance);
}

//
// The STL equivalent of the forward function. Same signature and behaviour.
// https://en.cppreference.com/w/cpp/utility/forward
//
template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>&& instance) noexcept
{
    return static_cast<T&&>(instance);
}

template<typename TypeIfTrue, typename TypeIfFalse, bool condition>
using ConditionalType = typename Detail::ConditionalType<TypeIfTrue, TypeIfFalse, condition>::Type;

template<typename DerivedType, typename BaseType>
constexpr bool is_derived_from = std::is_base_of_v<BaseType, DerivedType>;

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::forward;
using AT::i16;
using AT::i32;
using AT::i64;
using AT::i8;
using AT::invalid_size;
using AT::invalid_unicode_codepoint;
using AT::is_integral;
using AT::is_signed_integral;
using AT::is_unsigned_integral;
using AT::move;
using AT::ReadonlyByte;
using AT::ReadonlyBytes;
using AT::ReadWriteByte;
using AT::ReadWriteBytes;
using AT::RemoveConst;
using AT::RemoveReference;
using AT::ssize;
using AT::u16;
using AT::u32;
using AT::u64;
using AT::u8;
using AT::uintptr;
using AT::UnicodeCodepoint;
using AT::usize;
using AT::WriteonlyByte;
using AT::WriteonlyBytes;
#endif // AT_INCLUDE_GLOBALLY
