/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#ifdef _WIN32
    #define AT_PLATFORM_WINDOWS 1
    #ifndef _WIN64
        #error AT can only be compiled for Windows 64-bit!
    #endif // _WIN64
#endif // _WIN32

#if defined(__linux__) || defined(__gnu_linux__)
    #define AT_PLATFORM_LINUX 1
#endif // __linux__ || __gnu_linux__

#ifdef __APPLE__
    #include <TargetConditionals.h>
    #if TARGET_OS_MAC
        #define AT_PLATFORM_MACOS 1
    #else
        #error The only Apple platform AT supports is macOS!
    #endif // TARGET_OS_MAC
#endif // __APPLE__

#ifndef AT_PLATFORM_WINDOWS
    #define AT_PLATFORM_WINDOWS 0
#endif // AT_PLATFORM_WINDOWS

#ifndef AT_PLATFORM_LINUX
    #define AT_PLATFORM_LINUX 0
#endif // AT_PLATFORM_LINUX

#ifndef AT_PLATFORM_MACOS
    #define AT_PLATFORM_MACOS 0
#endif // AT_PLATFORM_MACOS

#if !AT_PLATFORM_WINDOWS && !AT_PLATFORM_LINUX && !AT_PLATFORM_MACOS
    #error Unknown or unsupported platform!
#endif // Any desktop supported platform.

#ifdef _DEBUG
    #define AT_CONFIGURATION_DEBUG   1
    #define AT_CONFIGURATION_RELEASE 0
#else
    #define AT_CONFIGURATION_DEBUG   0
    #define AT_CONFIGURATION_RELEASE 1
#endif // _DEBUG

#if defined(_MSC_BUILD) && !defined(__clang__)
    #define AT_COMPILER_MSVC  1
    #define AT_COMPILER_CLANG 0
    #define AT_COMPILER_GCC   0
#endif // _MSC_BUILD && !__clang__

#ifdef __clang__
    #define AT_COMPILER_MSVC  0
    #define AT_COMPILER_CLANG 1
    #define AT_COMPILER_GCC   0
#endif // __clang__

#if defined(__GNUC__) && !defined(__clang__)
    #define AT_COMPILER_MSVC  0
    #define AT_COMPILER_CLANG 0
    #define AT_COMPILER_GCC   1
#endif // __GNUC__ && !__clang__

#if !AT_COMPILER_MSVC && !AT_COMPILER_CLANG && !AT_COMPILER_GCC
    #error Unknown or unsupported compiler!
#endif // Any supported compiler.

#define NODISCARD    [[nodiscard]]
#define MAYBE_UNUSED [[maybe_unused]]
#define LIKELY       [[likely]]
#define AT_FILE      __FILE__
#define AT_LINE      __LINE__

#if AT_COMPILER_MSVC
    #define ALWAYS_INLINE __forceinline
    #define AT_FUNCTION   __FUNCSIG__
    #define AT_DEBUGBREAK __debugbreak()
#elif AT_COMPILER_CLANG
    #define ALWAYS_INLINE __attribute__((always_inline))
    #define AT_FUNCTION   __PRETTY_FUNCTION__
    #define AT_DEBUGBREAK __builtin_trap()
#elif AT_COMPILER_GCC
    #define ALWAYS_INLINE __attribute__((always_inline))
    #define AT_FUNCTION   __PRETTY_FUNCTION__
    #define AT_DEBUGBREAK __builtin_trap()
#endif // Compiler switch.

#define AT_STRINGIFY_IMPL(x)      #x
#define AT_CONCATENATE_IMPL(x, y) x##y

#define AT_STRINGIFY(x)      AT_STRINGIFY_IMPL(x)
#define AT_CONCATENATE(x, y) AT_CONCATENATE_IMPL(x, y)

#if AT_PLATFORM_WINDOWS
    #if AT_COMPILER_GCC
        #define AT_API_SPECIFIER_EXPORT __attribute__((visibility("default")))
        #define AT_API_SPECIFIER_IMPORT
    #else
        #define AT_API_SPECIFIER_EXPORT __declspec(dllexport)
        #define AT_API_SPECIFIER_IMPORT __declspec(dllimport)
    #endif // AT_COMPILER_GCC
#else
    #define AT_API_SPECIFIER_EXPORT __attribute__((visibility("default")))
    #define AT_API_SPECIFIER_IMPORT
#endif // AT_PLATFORM_WINDOWS

#ifdef AT_EXPORT_API
    #define AT_API AT_API_SPECIFIER_EXPORT
#else
    #define AT_API AT_API_SPECIFIER_IMPORT
#endif // AT_EXPORT_API