/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreDefines.h"
#include "AT/CoreTypes.h"
#include "AT/Error.h"
#include "AT/Span.h"

namespace AT {

//
// A view towards a UTF-8 encoded string.
// The held string is not null-terminated and can't be mutated by the string view.
//
class StringView {
public:
    AT_API static ErrorOr<StringView> create_from_utf8(const char* characters, usize byte_count);
    AT_API static ErrorOr<StringView> create_from_utf8(const char* null_terminated_characters);

    NODISCARD ALWAYS_INLINE static constexpr StringView
    unsafe_create_from_utf8(const char* characters, usize byte_count)
    {
        StringView view;
        view.m_characters = characters;
        view.m_byte_count = byte_count;
        return view;
    }

public:
    ALWAYS_INLINE constexpr StringView()
        : m_characters(nullptr)
        , m_byte_count(0)
    {}

    StringView(const StringView&) = default;
    StringView& operator=(const StringView&) = default;

    ALWAYS_INLINE constexpr StringView(StringView&& other) noexcept
        : m_characters(other.m_characters)
        , m_byte_count(other.m_byte_count)
    {
        other.m_characters = nullptr;
        other.m_byte_count = 0;
    }

    ALWAYS_INLINE constexpr StringView& operator=(StringView&& other) noexcept
    {
        m_characters = other.m_characters;
        m_byte_count = other.m_byte_count;
        other.m_characters = nullptr;
        other.m_byte_count = 0;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span() const
    {
        return ReadonlyByteSpan(reinterpret_cast<ReadonlyBytes>(m_characters), m_byte_count);
    }

private:
    const char* m_characters;
    usize m_byte_count;
};

#if AT_COMPILER_MSVC
    #pragma warning(push)
    // Disables the following compiler warning:
    // 'operator ""sv': literal suffix identifiers that do not start with an underscore are reserved.
    #pragma warning(disable : 4455)
#elif AT_COMPILER_CLANG
    #pragma clang diagnostic push
// Disables the following compiler warning:
// warning: user-defined literal suffixes not starting with '_' are reserved.
    #pragma clang diagnostic ignored "-Wuser-defined-literals"
#elif AT_COMPILER_GCC
    #pragma GCC diagnostic push
// Disables the following compiler warning:
// : literal operator suffixes not preceded by '_' are reserved for future standardization
    #pragma GCC diagnostic ignored "-Wliteral-suffix"
#endif // Compiler enumeration.

NODISCARD ALWAYS_INLINE constexpr StringView operator""sv(const char* ascii_literal, usize ascii_literal_count)
{
    // NOTE: Because ASCII is a subset of UTF-8, no conversion is needed.
    //       Also, we can be sure that the string literals are valid, unless something truly
    //       horrible happened to the project source files! :)
    return StringView::unsafe_create_from_utf8(ascii_literal, ascii_literal_count);
}

#if AT_COMPILER_MSVC
    #pragma warning(pop)
#elif AT_COMPILER_CLANG
    #pragma clang diagnostic pop
#elif AT_COMPILER_GCC
    #pragma GCC diagnostic pop
#endif // Compiler enumeration.

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::StringView;
#endif // AT_INCLUDE_GLOBALLY