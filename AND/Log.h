/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/StringBuilder.h"
#include "AND/Utf8String.h"

namespace AND {

void dbgln_impl(Utf8View);
void outln_impl(Utf8View);
void warnln_impl(Utf8View);
void errorln_impl(Utf8View);

template<typename... Args>
void dbgln(char const* message, Args&&... args)
{
    StringBuilder builder;
    builder.append_formatted(Utf8View::from_null_terminated(message), forward<Args>(args)...);
    Utf8String formatted = builder.build_utf8();
    dbgln_impl(formatted.view());
}

template<typename... Args>
void outln(char const* message, Args&&... args)
{
    StringBuilder builder;
    builder.append_formatted(Utf8View::from_null_terminated(message), forward<Args>(args)...);
    Utf8String formatted = builder.build_utf8();
    outln_impl(formatted.view());
}

template<typename... Args>
void warnln(char const* message, Args&&... args)
{
    StringBuilder builder;
    builder.append_formatted(Utf8View::from_null_terminated(message), forward<Args>(args)...);
    Utf8String formatted = builder.build_utf8();
    warnln_impl(formatted.view());
}

template<typename... Args>
void errorln(char const* message, Args&&... args)
{
    StringBuilder builder;
    builder.append_formatted(Utf8View::from_null_terminated(message), forward<Args>(args)...);
    Utf8String formatted = builder.build_utf8();
    errorln_impl(formatted.view());
}

#define dbgln_if(condition, ...)       \
    do {                               \
        if constexpr (condition)       \
            ::AND::dbgln(__VA_ARGS__); \
    } while (false)

#define outln_if(condition, ...)       \
    do {                               \
        if constexpr (condition)       \
            ::AND::outln(__VA_ARGS__); \
    } while (false)

#define warnln_if(condition, ...)       \
    do {                                \
        if constexpr (condition)        \
            ::AND::warnln(__VA_ARGS__); \
    } while (false)

#define errorln_if(condition, ...)       \
    do {                                 \
        if constexpr (condition)         \
            ::AND::errorln(__VA_ARGS__); \
    } while (false)

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::dbgln;
using AND::errorln;
using AND::outln;
using AND::warnln;
#endif
