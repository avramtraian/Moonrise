/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

enum class AssertionKind {
    Assert,
    AssertNotReached,
    Panic,
};

void on_assertion_failed(AssertionKind, char const* expression, char const* file, char const* function, int line, char const* message);

} // namespace AND

#define ASSERT(...)                                                                                                            \
    do {                                                                                                                       \
        if (!(__VA_ARGS__)) {                                                                                                  \
            ::AND::on_assertion_failed(::AND::AssertionKind::Assert, #__VA_ARGS__, AND_FILE, AND_FUNCTION, AND_LINE, nullptr); \
            AND_DEBUGBREAK;                                                                                                    \
        }                                                                                                                      \
    } while (false)

#define ASSERTF(expression, message)                                                                                          \
    do {                                                                                                                      \
        if (!(expression)) {                                                                                                  \
            ::AND::on_assertion_failed(::AND::AssertionKind::Assert, #expression, AND_FILE, AND_FUNCTION, AND_LINE, message); \
            AND_DEBUGBREAK;                                                                                                   \
        }                                                                                                                     \
    } while (false)

#define ASSERT_NOT_REACHED                                                                                                      \
    do {                                                                                                                        \
        ::AND::on_assertion_failed(::AND::AssertionKind::AssertNotReached, nullptr, AND_FILE, AND_FUNCTION, AND_LINE, nullptr); \
        AND_DEBUGBREAK;                                                                                                         \
    } while (false)

#define ASSERTF_NOT_REACHED(message)                                                                                            \
    do {                                                                                                                        \
        ::AND::on_assertion_failed(::AND::AssertionKind::AssertNotReached, nullptr, AND_FILE, AND_FUNCTION, AND_LINE, message); \
        AND_DEBUGBREAK;                                                                                                         \
    } while (false)

#define PANIC(message)                                                                                               \
    do {                                                                                                             \
        ::AND::on_assertion_failed(::AND::AssertionKind::Panic, nullptr, AND_FILE, AND_FUNCTION, AND_LINE, message); \
        AND_DEBUGBREAK;                                                                                              \
    } while (false)
