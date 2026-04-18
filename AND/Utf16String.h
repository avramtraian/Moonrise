/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Forward.h"
#include "AND/Types.h"
#include "AND/Utf16View.h"

namespace AND {

class Utf16String {
public:
    friend class StringBuilder;

    struct HeapBlock {
        u32 reference_count { 0 };
        u16 code_units[];
    };

    static constexpr usize inline_capacity = sizeof(HeapBlock*) / sizeof(u16);
    static_assert(inline_capacity > 0);

public:
    NODISCARD static Utf16String from_utf8(StringView const&);

public:
    Utf16String();
    ~Utf16String();

    Utf16String(Utf16String const&);
    Utf16String(Utf16String&&) noexcept;
    /*implicit*/ Utf16String(Utf16View);

    Utf16String& operator=(Utf16String const&);
    Utf16String& operator=(Utf16String&&) noexcept;
    Utf16String& operator=(Utf16View);

public:
    NODISCARD ALWAYS_INLINE u16 const* code_units() const { return is_stored_inline() ? m_inline_buffer : m_heap_block->code_units; }
    NODISCARD ALWAYS_INLINE usize code_unit_count() const { return m_code_unit_count; }
    NODISCARD ALWAYS_INLINE usize code_unit_count_without_null_terminator() const
    {
        ASSERT(m_code_unit_count >= 1);
        return m_code_unit_count - 1;
    }

    NODISCARD ALWAYS_INLINE ConstSpan<u16> code_units_span() const { return ConstSpan { code_units(), code_unit_count() }; }
    NODISCARD ALWAYS_INLINE ConstSpan<u16> code_units_span_without_null_terminator() const { return ConstSpan { code_units(), code_unit_count_without_null_terminator() }; }

    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_code_unit_count <= 1); }

    NODISCARD ALWAYS_INLINE ROBytes bytes() const { return reinterpret_cast<ROBytes>(code_units()); }
    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_code_unit_count * sizeof(u16); }
    NODISCARD ALWAYS_INLINE ROByteSpan byte_span() const { return ROByteSpan { bytes(), byte_count() }; }

    NODISCARD ALWAYS_INLINE Utf16View view() const { return Utf16View::from_code_units(code_units(), code_unit_count_without_null_terminator()); }
    NODISCARD ALWAYS_INLINE Utf16View view_with_null_terminator() const { return Utf16View::from_code_units(code_units(), code_unit_count()); }

public:
    void clear();

    NODISCARD ALWAYS_INLINE bool validate() const { return view().validate(); }
    NODISCARD ALWAYS_INLINE usize calculate_length() const { return view().calculate_length(); }

private:
    NODISCARD ALWAYS_INLINE bool is_stored_inline() const { return (m_code_unit_count <= inline_capacity); }
    NODISCARD ALWAYS_INLINE bool is_stored_on_heap() const { return (m_code_unit_count > inline_capacity); }

    NODISCARD static HeapBlock* allocate_memory(usize in_code_unit_count);
    static void free_memory(HeapBlock* block, usize in_code_unit_count);

private:
    usize m_code_unit_count;
    union {
        HeapBlock* m_heap_block;
        u16 m_inline_buffer[inline_capacity];
    };
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::Utf16String;
#endif
