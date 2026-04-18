/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Assertions.h"
#include "AND/Span.h"
#include "AND/StringBuilder.h"
#include "AND/StringView.h"
#include "AND/Types.h"

namespace AND {

class String {
public:
    friend class StringBuilder;

    struct HeapBlock {
        u32 reference_count { 0 };
        RWByte buffer[];
    };

    static constexpr usize inline_capacity = sizeof(HeapBlock*);
    static_assert(inline_capacity > 0);

public:
    NODISCARD static String from_utf16(Utf16View const&);

    NODISCARD static String number_unsigned(u64 value);
    NODISCARD static String number_signed(s64 value);
    NODISCARD static String number_float(f64 value);

    template<typename... Args>
    NODISCARD ALWAYS_INLINE static String format(StringView format_string, Args&&... args)
    {
        StringBuilder builder;
        builder.set_encoding(StringBuilder::Encoding::UTF8);
        builder.append_formatted(format_string, forward<Args>(args)...);
        return builder.build_utf8();
    }

public:
    String();
    ~String();

    String(String const&);
    String(String&&) noexcept;
    /*implicit*/ String(StringView);

    String& operator=(String const&);
    String& operator=(String&&) noexcept;
    String& operator=(StringView);

public:
    NODISCARD ALWAYS_INLINE ROBytes bytes() const { return is_stored_inline() ? m_inline_buffer : m_heap_block->buffer; }
    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_byte_count; }
    NODISCARD ALWAYS_INLINE usize byte_count_without_null_terminator() const
    {
        ASSERT(m_byte_count >= sizeof('\0'));
        return m_byte_count - sizeof('\0');
    }

    NODISCARD ALWAYS_INLINE ROByteSpan byte_span() const { return ROByteSpan { bytes(), byte_count() }; }
    NODISCARD ALWAYS_INLINE ROByteSpan byte_span_without_null_terminator() const { return ROByteSpan { bytes(), byte_count_without_null_terminator() }; }

    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_byte_count <= sizeof('\0')); }

    NODISCARD ALWAYS_INLINE StringView view() const { return StringView::from_unverified(bytes(), byte_count_without_null_terminator()); }
    NODISCARD ALWAYS_INLINE StringView view_with_null_terminator() const { return StringView::from_unverified(bytes(), byte_count()); }

public:
    void clear();

    NODISCARD ALWAYS_INLINE bool validate() const { return view().validate(); }
    NODISCARD ALWAYS_INLINE usize calculate_length() const { return view().calculate_length(); }

private:
    NODISCARD ALWAYS_INLINE bool is_stored_inline() const { return (m_byte_count <= inline_capacity); }
    NODISCARD ALWAYS_INLINE bool is_stored_on_heap() const { return (m_byte_count > inline_capacity); }

    NODISCARD static HeapBlock* allocate_memory(usize in_byte_count);
    static void free_memory(HeapBlock* block, usize in_byte_count);

private:
    usize m_byte_count;
    union {
        HeapBlock* m_heap_block;
        RWByte m_inline_buffer[inline_capacity];
    };
};

void append_to_builder(StringBuilder&, Optional<StringView>, String const&);

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::String;
#endif
