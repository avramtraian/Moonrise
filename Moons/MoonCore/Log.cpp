/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "MoonCore/Log.h"
#include <cstdio>

namespace Core {

static bool s_disable_debug_logs = false;
static bool s_disable_warning_logs = false;
static bool s_disable_error_logs = false;

void dbgln(StringView message)
{
    if (!s_disable_debug_logs) {
        auto message_span = message.byte_span().as<const char>();
        printf("%.*s\n", static_cast<int>(message_span.count()), message_span.elements());
    }
}

void warnln(StringView message)
{
    if (!s_disable_warning_logs) {
        auto message_span = message.byte_span().as<const char>();
        // TODO: Set the color of the 'WARNING' header to yellow.
        printf("WARNING: %.*s\n", static_cast<int>(message_span.count()), message_span.elements());
    }
}

void errorln(StringView message)
{
    if (!s_disable_error_logs) {
        auto message_span = message.byte_span().as<const char>();
        // TODO: Set the color of the 'ERROR' header to red.
        printf("ERROR: %.*s\n", static_cast<int>(message_span.count()), message_span.elements());
    }
}

} // namespace Core
