/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/Format.h"

#define AT_FORMAT_SPECIFIER_BEGIN_TOKEN '{'
#define AT_FORMAT_SPECIFIER_END_TOKEN   '}'

namespace AT {

ErrorOr<String> FormatBuilder::release_string()
{
    // NOTE: Creating a string from a vector of characters is very wasteful. In our current implementation,
    //       there is always an unnecessary memory allocation. If the formatted string fits in a String
    //       inline capacity then allocating the memory for the vector is not necessary. Otherwise, memory
    //       is allocated for both the vector and the released string even though the vector memory is
    //       freed right after.
    // TODO: Replace the Vector container to something else that minimizes unnecessary memory allocations.
    //       At the moment of writing this implementation, the concept of a StringBuilder doesn't exist.

    auto formatted_view =
        StringView::unsafe_create_from_utf8(m_formatted_string_buffer.elements(), m_formatted_string_buffer.count());
    TRY_ASSIGN(auto formatted_string, String::create(formatted_view));
    m_formatted_string_buffer.clear_and_shrink();
    return formatted_string;
}

ErrorOr<void> FormatBuilder::consume_until_format_specifier()
{
    usize specifier_offset = m_string_format.find(AT_FORMAT_SPECIFIER_BEGIN_TOKEN);
    if (specifier_offset == StringView::invalid_position) {
        TRY(m_formatted_string_buffer.try_add_span(m_string_format.byte_span().as<const char>()));
        return {};
    }

    TRY(m_formatted_string_buffer.try_add_span(m_string_format.byte_span().slice(0, specifier_offset).as<const char>())
    );
    m_string_format = m_string_format.slice(specifier_offset);
    return {};
}

ErrorOr<FormatBuilder::Specifier> FormatBuilder::parse_specifier()
{
    if (m_string_format.is_empty() || m_string_format.byte_span()[0] != AT_FORMAT_SPECIFIER_BEGIN_TOKEN) {
        // NOTE: This function is only called when there is an argument passed to the 'format()' function
        //       that hasn't been inserted in the formatted string yet. If this codepath is reached it
        //       means that more arguments were passed to the function than required.
        return Error::InvalidStringFormat;
    }

    // Advance the format string to the next character, ignoring the AT_FORMAT_SPECIFIER_BEGIN_TOKEN.
    m_string_format = m_string_format.slice(1);

    usize specifier_count = m_string_format.find(AT_FORMAT_SPECIFIER_END_TOKEN);
    if (specifier_count == StringView::invalid_position) {
        // NOTE: Because the format specifier doesn't have an end token the
        //       string format is considered invalid.
        return Error::InvalidStringFormat;
    }

    const StringView specifier_string = m_string_format.slice(0, specifier_count);
    m_string_format = m_string_format.slice(specifier_count + 1);

    TRY_ASSIGN(auto specifier, FormatBuilder::parse_specifier(specifier_string));
    return specifier;
}

ErrorOr<FormatBuilder::Specifier> FormatBuilder::parse_specifier(StringView specifier_string)
{
    if (!specifier_string.is_empty()) {
        // NOTE: Currently, we don't support any string format specifier!
        return Error::InvalidStringFormat;
    }

    return Specifier();
}

ErrorOr<void> FormatBuilder::push_unsigned_integer(const Specifier&, u64 value)
{
    // NOTE: The longest integer representation (in binary) is no longer than 64 characters.
    char buffer[64] = {};
    u8 digit_count = 0;

    if (value == 0) {
        buffer[0] = '0';
        digit_count = 1;
    }
    else {
        for (u64 temp_value = value; temp_value > 0; temp_value /= 10) {
            ++digit_count;
        }

        for (u8 digit_index = 0; digit_index < digit_count; ++digit_index) {
            u8 digit = value % 10;
            buffer[digit_count - digit_index - 1] = '0' + digit;
            value /= 10;
        }
    }

    TRY(m_formatted_string_buffer.try_add_span({ buffer, digit_count }));
    return {};
}

ErrorOr<void> FormatBuilder::push_signed_integer(const Specifier& specifier, i64 value)
{
    if (value < 0) {
        TRY(m_formatted_string_buffer.try_add('-'));
        value = -value;
    }

    TRY(push_unsigned_integer(specifier, static_cast<u64>(value)));
    return {};
}

ErrorOr<void> FormatBuilder::push_string(const Specifier&, StringView value)
{
    TRY(m_formatted_string_buffer.try_add_span(value.byte_span().as<const char>()));
    return {};
}

} // namespace AT