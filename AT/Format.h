/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreDefines.h"
#include "AT/CoreTypes.h"
#include "AT/Error.h"
#include "AT/Span.h"
#include "AT/String.h"
#include "AT/Vector.h"

namespace AT {

class FormatBuilder {
    AT_MAKE_NONCOPYABLE(FormatBuilder);
    AT_MAKE_NONMOVABLE(FormatBuilder);

public:
    struct Specifier {};

public:
    ALWAYS_INLINE FormatBuilder(StringView string_format)
        : m_string_format(string_format)
    {}

public:
    NODISCARD AT_API ErrorOr<String> release_string();

    AT_API ErrorOr<void> consume_until_format_specifier();
    AT_API ErrorOr<Specifier> parse_specifier();
    static ErrorOr<Specifier> parse_specifier(StringView specifier_string);

    AT_API ErrorOr<void> push_unsigned_integer(const Specifier& specifier, u64 value);
    AT_API ErrorOr<void> push_signed_integer(const Specifier& specifier, i64 value);

private:
    StringView m_string_format;
    Vector<char> m_formatted_string_buffer;
};

template<typename T>
struct Formatter {
public:
    ALWAYS_INLINE static ErrorOr<void>
    format(FormatBuilder& builder, const FormatBuilder::Specifier& specifier, const T& value)
    {
        // TODO: Maybe we should signal that the Formatter<T> isn't specialized by
        //       gracefully returning an error code?
        AT_ASSERT(false);
        return {};
    }
};

namespace Detail {

NODISCARD ALWAYS_INLINE ErrorOr<void> format(FormatBuilder& builder)
{
    TRY(builder.consume_until_format_specifier());
    return {};
}

template<typename T, typename... Args>
NODISCARD ALWAYS_INLINE ErrorOr<void> format(FormatBuilder& builder, const T& argument, Args&&... other_arguments)
{
    TRY(builder.consume_until_format_specifier());
    TRY_ASSIGN(auto format_specifier, builder.parse_specifier());

    TRY(Formatter<T>::format(builder, format_specifier, argument));

    TRY(format(builder, forward<Args>(other_arguments)...));
    return {};
}

} // namespace Detail

template<typename... Args>
NODISCARD ALWAYS_INLINE ErrorOr<String> format(StringView string_format, Args&&... args)
{
    auto builder = FormatBuilder(string_format);
    TRY(Detail::format(builder, forward<Args>(args)...));
    TRY_ASSIGN(auto formatted_string, builder.release_string());
    return formatted_string;
}

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::format;
using AT::FormatBuilder;
using AT::Formatter;
#endif // AT_INCLUDE_GLOBALLY