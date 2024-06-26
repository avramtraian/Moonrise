/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Format.h>
#include <AT/StringView.h>
#include <MoonCore/Core.h>

namespace Core {

CORE_API void dbgln(StringView message);
CORE_API void warnln(StringView message);
CORE_API void errorln(StringView message);

template<typename... Args>
ALWAYS_INLINE void dbgln(StringView message, Args&&... args)
{
    auto formatted_message_or_error = format(message, forward<Args>(args)...);
    if (!formatted_message_or_error.has_value()) {
        // NOTE: Asserting for failing to format the message in a log would be very excessive.
        return;
    }

    String formatted_message = formatted_message_or_error.release_value();
    dbgln(formatted_message.view());
}

template<typename... Args>
ALWAYS_INLINE void dbgln(const char* null_terminated_message, Args&&... args)
{
    const StringView message = StringView::create_from_utf8(null_terminated_message);
    dbgln(message, forward<Args>(args)...);
}

template<typename... Args>
ALWAYS_INLINE void warnln(StringView message, Args&&... args)
{
    auto formatted_message_or_error = format(message, forward<Args>(args)...);
    if (!formatted_message_or_error.has_value()) {
        // NOTE: Asserting for failing to format the message in a log would be very excessive.
        return;
    }

    String formatted_message = formatted_message_or_error.release_value();
    warnln(formatted_message.view());
}

template<typename... Args>
ALWAYS_INLINE void warnln(const char* null_terminated_message, Args&&... args)
{
    const StringView message = StringView::create_from_utf8(null_terminated_message);
    warnln(message, forward<Args>(args)...);
}

template<typename... Args>
ALWAYS_INLINE void errorln(StringView message, Args&&... args)
{
    auto formatted_message_or_error = format(message, forward<Args>(args)...);
    if (!formatted_message_or_error.has_value()) {
        // NOTE: Asserting for failing to format the message in a log would be very excessive.
        return;
    }

    String formatted_message = formatted_message_or_error.release_value();
    errorln(formatted_message.view());
}

template<typename... Args>
ALWAYS_INLINE void errorln(const char* null_terminated_message, Args&&... args)
{
    const StringView message = StringView::create_from_utf8(null_terminated_message);
    errorln(message, forward<Args>(args)...);
}

} // namespace Core

using Core::dbgln;
using Core::errorln;
using Core::warnln;
