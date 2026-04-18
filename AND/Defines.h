/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

//==============================================================================
// PLATFORM OPERATING SYSTEM.
//==============================================================================

#ifdef _WIN32
#    define AND_PLATFORM_OS_WINDOWS 1
#endif

#ifndef AND_PLATFORM_OS_WINDOWS
#    define AND_PLATFORM_OS_WINDOWS 0
#endif

#ifndef AND_PLATFORM_OS_LINUX
#    define AND_PLATFORM_OS_LINUX 0
#endif

#ifndef AND_PLATFORM_OS_MACOS
#    define AND_PLATFORM_OS_MACOS 0
#endif

#define AND_PLATFORM_OS_COUNT (AND_PLATFORM_OS_WINDOWS + AND_PLATFORM_OS_LINUX + AND_PLATFORM_OS_MACOS)
#if AND_PLATFORM_OS_COUNT == 0
#    error "Unknown or unsupported operating system!"
#endif
#if AND_PLATFORM_OS_COUNT > 1
#    error "Multiple operating systems detected!"
#endif
#undef AND_PLATFORM_OS_COUNT

//==============================================================================
// PLATFORM ARCHITECTURE.
//==============================================================================

#if AND_PLATFORM_OS_WINDOWS
#    ifdef _WIN64
#        define AND_PLATFORM_ARCH_X64 1
#    else
#        define AND_PLATFORM_ARCH_X86 1
#    endif
#endif

#ifndef AND_PLATFORM_ARCH_X86
#    define AND_PLATFORM_ARCH_X86 0
#endif

#ifndef AND_PLATFORM_ARCH_X64
#    define AND_PLATFORM_ARCH_X64 0
#endif

#ifndef AND_PLATFORM_ARCH_ARM32
#    define AND_PLATFORM_ARCH_ARM32 0
#endif

#ifndef AND_PLATFORM_ARCH_ARM64
#    define AND_PLATFORM_ARCH_ARM64 0
#endif

#define AND_PLATFORM_ARCH_COUNT (AND_PLATFORM_ARCH_X86 + AND_PLATFORM_ARCH_X64 + AND_PLATFORM_ARCH_ARM32 + AND_PLATFORM_ARCH_ARM64)
#if AND_PLATFORM_ARCH_COUNT == 0
#    error "Unknown or unsupported architecture!"
#endif
#if AND_PLATFORM_ARCH_COUNT > 1
#    error "Multiple architectures detected!"
#endif
#undef AND_PLATFORM_ARCH_COUNT

//==============================================================================
// PLATFORM COMPILER.
//==============================================================================

#ifdef __clang__
#    define AND_PLATFORM_COMPILER_CLANG 1
#else
#    ifdef _MSC_VER
#        define AND_PLATFORM_COMPILER_MSVC 1
#    endif

#    ifdef __GNUC__
#        define AND_PLATFORM_COMPILER_GCC 1
#    endif
#endif

#ifndef AND_PLATFORM_COMPILER_CLANG
#    define AND_PLATFORM_COMPILER_CLANG 0
#endif

#ifndef AND_PLATFORM_COMPILER_MSVC
#    define AND_PLATFORM_COMPILER_MSVC 0
#endif

#ifndef AND_PLATFORM_COMPILER_GCC
#    define AND_PLATFORM_COMPILER_GCC 0
#endif

#define AND_PLATFORM_COMPILER_COUNT (AND_PLATFORM_COMPILER_CLANG + AND_PLATFORM_COMPILER_MSVC + AND_PLATFORM_COMPILER_GCC)
#if AND_PLATFORM_COMPILER_COUNT == 0
#    error "Unknown or unsupported compiler!"
#endif
#if AND_PLATFORM_COMPILER_COUNT > 1
#    error "Multiple compilers detected!"
#endif
#undef AND_PLATFORM_COMPILER_COUNT

//==============================================================================
// PLATFORM ENDIANESS.
//==============================================================================

#ifdef __BYTE_ORDER__
#    if defined(__ORDER_LITTLE_ENDIAN__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#        define AND_PLATFORM_ENDIANESS_LITTLE 1
#    endif
#    if defined(__ORDER_BIG_ENDIAN__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#        define AND_PLATFORM_ENDIANESS_BIG 1
#    endif
#endif

#ifndef AND_PLATFORM_ENDIANESS_LITTLE
#    define AND_PLATFORM_ENDIANESS_LITTLE 0
#endif

#ifndef AND_PLATFORM_ENDIANESS_BIG
#    define AND_PLATFORM_ENDIANESS_BIG 0
#endif

#if AND_PLATFORM_ENDIANESS_LITTLE || AND_PLATFORM_ENDIANESS_BIG
#    define AND_PLATFORM_ENDIANESS_UNKNOWN 0
#else
#    define AND_PLATFORM_ENDIANESS_UNKNOWN 1
#endif

#if AND_PLATFORM_ENDIANESS_LITTLE && AND_PLATFORM_ENDIANESS_BIG
#    error "Multiple endianesses detected!"
#endif

//==============================================================================
// BUILD CONFIGURATION.
//==============================================================================

#ifndef AND_CONFIGURATION_DEBUG
#    define AND_CONFIGURATION_DEBUG 0
#endif

#ifndef AND_CONFIGURATION_RELEASE
#    define AND_CONFIGURATION_RELEASE 0
#endif

#if !AND_CONFIGURATION_DEBUG && !AND_CONFIGURATION_RELEASE
#    ifdef _DEBUG
#        define AND_CONFIGURATION_DEBUG 1
#        define AND_CONFIGURATION_RELEASE 0
#    else
#        define AND_CONFIGURATION_DEBUG 0
#        define AND_CONFIGURATION_RELEASE 1
#    endif
#endif

#define AND_CONFIGURATION_COUNT (AND_CONFIGURATION_DEBUG + AND_CONFIGURATION_RELEASE)
#if AND_CONFIGURATION_COUNT == 0
#    error "Unknown or unsupported build configuration!"
#endif
#if AND_CONFIGURATION_COUNT > 1
#    error "Multiple build configurations detected!"
#endif
#undef AND_CONFIGURATION_COUNT

//==============================================================================
// PLATFORM-AGNOSTIC UTILITY MACROS.
//==============================================================================

#define NODISCARD [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]
#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]

#define ARRAY_COUNT(x) (sizeof(x) / sizeof(x[0]))
#define OFFSET_OF(type, member) (reinterpret_cast<::AND::usize>(&reinterpret_cast<type*>(0xFF)->member) - 0xFF)

#define KiB(x) (1024 * (x))
#define MiB(x) (1024 * KiB(x))
#define GiB(x) (1024 * MiB(x))

#define AND_LINE __LINE__
#define AND_FILE __FILE__
#define AND_TIME __TIME__
#define AND_DATE __DATE__

#define AND_STRINGIFY_IMPL(x) #x
#define AND_STRINGIFY(x) AND_STRINGIFY_IMPL(x)

#define AND_CONCAT_IMPL(x, y) x##y
#define AND_CONCAT(x, y) AND_CONCAT_IMPL(x, y)

//==============================================================================
// PLATFORM-SPECIFIC UTILITY MACROS.
//==============================================================================

#if AND_PLATFORM_COMPILER_CLANG || AND_PLATFORM_COMPILER_GCC
#    define AND_FUNCTION __PRETTY_FUNCTION__
#    define AND_DEBUGBREAK __builtin_trap()
#    define ALWAYS_INLINE __attribute__((always_inline)) inline
#    define NEVER_INLINE __attribute__((noinline))
#    define NORETURN __attribute__((noreturn))
#endif

#if AND_PLATFORM_COMPILER_MSVC
#    define AND_FUNCTION __FUNCSIG__
#    define AND_DEBUGBREAK __debugbreak()
#    define ALWAYS_INLINE __forceinline
#    define NEVER_INLINE __declspec(noinline)
#    define NORETURN __declspec(noreturn)
#endif

//==============================================================================
// PLATFORM-SPECIFIC COMPILATION FLAGS.
//==============================================================================

#if AND_PLATFORM_COMPILER_CLANG
#    pragma clang diagnostic ignored "-Wzero-length-array"
#    pragma clang diagnostic ignored "-Wc99-extensions"
#endif

#if AND_PLATFORM_COMPILER_GCC
#    pragma GCC diagnostic ignored "-Wzero-length-array"
#    pragma GCC diagnostic ignored "-Wc99-extensions"
#endif

#if AND_PLATFORM_COMPILER_MSVC
// nonstandard extension used: zero-sized array in struct/union
#    pragma warning(disable : 4200)
#endif

//==============================================================================
// CONFIGURATION MACROS.
//==============================================================================

#define AND_INCLUDE_IN_GLOBAL_NAMESPACE 1
