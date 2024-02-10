/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Error.h"
#include "AT/Span.h"
#include "AT/StringView.h"

namespace AT {

//
// Container that stores a UTF-8 encoded, null-terminated string.
// If the string is small enough, the threshold being determined by the platform
// architecture, no memory will be allocated from the heap and the characters will
// be instead stored inline. This allows small strings to be very efficient in terms
// of performance, as copying and creating them would be very cheap.
//
class String {
public:
    static constexpr usize inline_capacity = sizeof(char*);
    static_assert(inline_capacity > 0);

public:
    AT_API static ErrorOr<String> create(StringView view);
    AT_API static ErrorOr<String> create(const String& other);
    AT_API static ErrorOr<String> create_from_utf8(const char* characters, usize byte_count);
    AT_API static ErrorOr<String> create_from_utf8(const char* null_terminated_characters);

public:
    AT_API String();
    AT_API ~String();

    AT_API String(const String& other);
    AT_API String(String&& other) noexcept;
    AT_API String(StringView string_view);

    AT_API String& operator=(const String& other);
    AT_API String& operator=(String&& other) noexcept;
    AT_API String& operator=(StringView string_view);

public:
    NODISCARD ALWAYS_INLINE StringView view() const
    {
        auto span = byte_span().as<const char>();
        // NOTE: We can be pretty sure that the held string is valid UTF-8, so checking
        //       for validity again would be a bit wasteful.
        return StringView::unsafe_create_from_utf8(span.elements(), span.count() * span.element_size());
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span()
    {
        AT_ASSERT(m_byte_count > 0);
        auto* bytes = reinterpret_cast<ReadWriteBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count - 1);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span() const
    {
        AT_ASSERT(m_byte_count > 0);
        const auto* bytes = reinterpret_cast<ReadonlyBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count - 1);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_byte_span()
    {
        AT_ASSERT(m_byte_count > 0);
        const auto* bytes = reinterpret_cast<ReadonlyBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count - 1);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span_with_null_termination()
    {
        AT_ASSERT(m_byte_count > 0);
        auto* bytes = reinterpret_cast<ReadWriteBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan byte_span_with_null_termination() const
    {
        AT_ASSERT(m_byte_count > 0);
        const auto* bytes = reinterpret_cast<ReadonlyBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count);
    }

    NODISCARD ALWAYS_INLINE ReadonlyByteSpan readonly_byte_span_with_null_termination()
    {
        AT_ASSERT(m_byte_count > 0);
        const auto* bytes = reinterpret_cast<ReadonlyBytes>(is_stored_inline() ? m_inline_buffer : m_heap_buffer);
        return ReadonlyByteSpan(bytes, m_byte_count);
    }

    NODISCARD ALWAYS_INLINE bool is_stored_inline() const { return m_byte_count <= inline_capacity; }
    NODISCARD ALWAYS_INLINE bool is_stored_on_heap() const { return m_byte_count > inline_capacity; }

private:
    NODISCARD static ErrorOr<char*> allocate_memory(usize byte_count);
    static ErrorOr<void> release_memory(char* heap_buffer, usize byte_count);

private:
    union {
        char m_inline_buffer[inline_capacity];
        char* m_heap_buffer;
    };
    usize m_byte_count;
};

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::String;
#endif // AT_INCLUDE_GLOBALLY