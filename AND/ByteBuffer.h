/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Assertions.h"
#include "AND/MemoryOperations.h"
#include "AND/Noncopyable.h"
#include "AND/Span.h"
#include "AND/Types.h"

namespace AND {

template<usize inline_capacity>
class InlineByteBuffer {
    AND_MAKE_NONCOPYABLE(InlineByteBuffer);

public:
    NODISCARD ALWAYS_INLINE static InlineByteBuffer allocate(usize in_byte_count)
    {
        InlineByteBuffer buffer;
        buffer.reallocate_without_copying(in_byte_count);
        return buffer;
    }

    NODISCARD ALWAYS_INLINE static InlineByteBuffer copy(ROByteSpan source)
    {
        InlineByteBuffer buffer = InlineByteBuffer::allocate(source.count());
        copy_memory(buffer.bytes(), source.elements(), source.count());
        return buffer;
    }

    NODISCARD ALWAYS_INLINE static InlineByteBuffer copy(InlineByteBuffer const& source)
    {
        return InlineByteBuffer::copy(source.byte_span());
    }

    template<usize source_inline_capacity>
    NODISCARD ALWAYS_INLINE static InlineByteBuffer copy(InlineByteBuffer<source_inline_capacity> const& source)
    {
        return InlineByteBuffer::copy(source.byte_span());
    }

public:
    ALWAYS_INLINE InlineByteBuffer()
        : m_byte_count(0)
        , m_inline_buffer {}
    {
    }

    ALWAYS_INLINE ~InlineByteBuffer()
    {
        release();
    }

    ALWAYS_INLINE InlineByteBuffer(InlineByteBuffer&& other) noexcept
        : m_byte_count(other.m_byte_count)
    {
        if (is_stored_inline())
            copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
        else
            m_heap_buffer = other.m_heap_buffer;

        other.m_byte_count = 0;
    }

    ALWAYS_INLINE InlineByteBuffer& operator=(InlineByteBuffer&& other) noexcept
    {
        // Handle the self-assignment case.
        if (this == &other)
            return *this;

        release();
        m_byte_count = other.m_byte_count;

        if (is_stored_inline())
            copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
        else
            m_heap_buffer = other.m_heap_buffer;

        other.m_byte_count = 0;
        return *this;
    }

public:
    NODISCARD ALWAYS_INLINE RWBytes bytes() { return is_stored_inline() ? m_inline_buffer : m_heap_buffer; }
    NODISCARD ALWAYS_INLINE ROBytes bytes() const { return is_stored_inline() ? m_inline_buffer : m_heap_buffer; }
    NODISCARD ALWAYS_INLINE ROBytes ro_bytes() const { return is_stored_inline() ? m_inline_buffer : m_heap_buffer; }

    NODISCARD ALWAYS_INLINE usize byte_count() const { return m_byte_count; }
    NODISCARD ALWAYS_INLINE bool is_empty() const { return (m_byte_count == 0); }

    NODISCARD ALWAYS_INLINE RWByteSpan byte_span() { return RWByteSpan { bytes(), m_byte_count }; }
    NODISCARD ALWAYS_INLINE ROByteSpan byte_span() const { return ROByteSpan { bytes(), m_byte_count }; }
    NODISCARD ALWAYS_INLINE ROByteSpan ro_byte_span() const { return ROByteSpan { ro_bytes(), m_byte_count }; }

public:
    ALWAYS_INLINE void
    release()
    {
        if (is_stored_on_heap())
            InlineByteBuffer::free_memory(m_heap_buffer, m_byte_count);
        m_byte_count = 0;
    }

    ALWAYS_INLINE void reallocate_without_copying(usize in_byte_count)
    {
        if (m_byte_count == in_byte_count)
            return;

        release();
        m_byte_count = in_byte_count;
        if (is_stored_on_heap())
            m_heap_buffer = InlineByteBuffer::allocate_memory(m_byte_count);
    }

private:
    NODISCARD ALWAYS_INLINE bool is_stored_inline() const { return (m_byte_count <= inline_capacity); }
    NODISCARD ALWAYS_INLINE bool is_stored_on_heap() const { return (m_byte_count > inline_capacity); }

    NODISCARD ALWAYS_INLINE static RWBytes allocate_memory(usize in_byte_count)
    {
        ASSERT(in_byte_count > inline_capacity);
        void* memory = operator new(in_byte_count);
        return static_cast<RWBytes>(memory);
    }

    ALWAYS_INLINE static void free_memory(RWBytes in_bytes, MAYBE_UNUSED usize in_byte_count)
    {
        ASSERT(in_byte_count > inline_capacity);
        operator delete(in_bytes);
    }

private:
    usize m_byte_count;
    union {
        RWBytes m_heap_buffer;
        RWByte m_inline_buffer[inline_capacity];
    };
};

using ByteBuffer = InlineByteBuffer<sizeof(RWBytes)>;

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::ByteBuffer;
#endif
