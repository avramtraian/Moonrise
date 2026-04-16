/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Assertions.h"
#include "AND/Span.h"
#include "AND/StringBuilder.h"
#include "AND/Types.h"
#include "AND/Utf8View.h"

namespace AND {

class Utf8String {
public:
    friend class StringBuilder;

    struct HeapBlock {
        u32 reference_count { 0 };
        RWByte buffer[];
    };

    static constexpr usize inline_capacity = sizeof(HeapBlock*);
    static_assert(inline_capacity > 0);

public:
    NODISCARD static Utf8String from_utf16(Utf16View const&);

    NODISCARD static Utf8String number_unsigned(u64 value);
    NODISCARD static Utf8String number_signed(s64 value);
    NODISCARD static Utf8String number_float(f64 value);

    template<typename... Args>
    NODISCARD ALWAYS_INLINE static Utf8String format(Utf8View format_string, Args&&... args)
    {
        StringBuilder builder;
        builder.set_encoding(StringBuilder::Encoding::UTF8);
        builder.append_formatted(format_string, forward<Args>(args)...);
        return builder.build_utf8();
    }

public:
    Utf8String();
    ~Utf8String();

    Utf8String(Utf8String const&);
    Utf8String(Utf8String&&) noexcept;
    /*implicit*/ Utf8String(Utf8View);

    Utf8String& operator=(Utf8String const&);
    Utf8String& operator=(Utf8String&&) noexcept;
    Utf8String& operator=(Utf8View);

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

    NODISCARD ALWAYS_INLINE Utf8View view() const { return Utf8View::from_unverified(bytes(), byte_count_without_null_terminator()); }
    NODISCARD ALWAYS_INLINE Utf8View view_with_null_terminator() const { return Utf8View::from_unverified(bytes(), byte_count()); }

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

void append_to_builder(StringBuilder&, Optional<Utf8View>, Utf8String const&);

} // namespace AND
