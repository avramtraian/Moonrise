/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreDefines.h"

// NOTE: std::initializer_list can't be implemented by us, so we include its header file here in
//       order to have access to it throughout the entire codebase without including it.
#include <initializer_list>

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
// If an mismatch is found, we do not allow the compilation to succeed, as
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
    type_name& operator=(const type_name&) = delete;

#define AT_MAKE_NONMOVABLE(type_name)         \
    type_name(type_name&&) noexcept = delete; \
    type_name& operator=(type_name&&) noexcept = delete;

namespace Internal {

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

} // namespace Internal

template<typename T>
using RemoveReference = typename Internal::RemoveReference<T>::Type;
template<typename T>
using RemoveConst = typename Internal::RemoveConst<T>::Type;

//
// The STL equivalent of the move function. Same signature and behaviour.
//
template<typename T>
NODISCARD ALWAYS_INLINE constexpr RemoveReference<T>&& move(T&& instance) noexcept
{
    return static_cast<RemoveReference<T>&&>(instance);
}

//
// The STL equivalent of the forward function. Same signature and behaviour.
//
template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>& instance) noexcept
{
    return static_cast<T&&>(instance);
}

//
// The STL equivalent of the forward function. Same signature and behaviour.
//
template<typename T>
NODISCARD ALWAYS_INLINE constexpr T&& forward(RemoveReference<T>&& instance) noexcept
{
    return static_cast<T&&>(instance);
}

} // namespace AT