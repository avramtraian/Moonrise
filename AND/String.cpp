/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/MemoryOperations.h"
#include "AND/String.h"

#include <new>

namespace AND {

String String::from_utf16(Utf16View const& utf16_view)
{
    StringBuilder builder;
    builder.set_encoding(StringBuilder::Encoding::UTF8);
    builder.append_utf16(utf16_view);
    return builder.build_utf8();
}

String String::number_unsigned(u64 value)
{
    StringBuilder builder;
    builder.set_encoding(StringBuilder::Encoding::UTF8);
    builder.append_unsigned(value, NumberBase::Decimal);
    return builder.build_utf8();
}

String String::number_signed(s64 value)
{
    StringBuilder builder;
    builder.set_encoding(StringBuilder::Encoding::UTF8);
    builder.append_signed(value, NumberBase::Decimal);
    return builder.build_utf8();
}

String String::number_float(f64 value)
{
    StringBuilder builder;
    builder.set_encoding(StringBuilder::Encoding::UTF8);
    builder.append_float(value);
    return builder.build_utf8();
}

String::String()
    : m_byte_count(sizeof('\0'))
{
    m_inline_buffer[0] = '\0';
}

String::~String()
{
    clear();
}

String::String(String const& other)
    : m_byte_count(other.m_byte_count)
{
    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
    } else {
        m_heap_block = other.m_heap_block;
        m_heap_block->reference_count++;
    }
}

String::String(String&& other) noexcept
    : m_byte_count(other.m_byte_count)
{
    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
    } else {
        m_heap_block = other.m_heap_block;
        other.m_heap_block = nullptr;
    }

    other.m_byte_count = sizeof('\0');
    zero_memory(other.m_inline_buffer, inline_capacity);
}

String::String(StringView view)
    : m_byte_count(view.byte_count() + sizeof('\0'))
{
    WOBytes destination = m_inline_buffer;
    if (is_stored_on_heap()) {
        m_heap_block = String::allocate_memory(m_byte_count);
        m_heap_block->reference_count = 1;
        destination = m_heap_block->buffer;
    }

    copy_memory(destination, view.bytes(), view.byte_count());
    destination[view.byte_count()] = '\0';
}

String& String::operator=(String const& other)
{
    // Handle the self-assignment case.
    if (this == &other)
        return *this;

    clear();
    m_byte_count = other.m_byte_count;

    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
    } else {
        m_heap_block = other.m_heap_block;
        m_heap_block->reference_count++;
    }

    return *this;
}

String& String::operator=(String&& other) noexcept
{
    // Handle the self-assignment case.
    if (this == &other)
        return *this;

    clear();
    m_byte_count = other.m_byte_count;

    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
    } else {
        m_heap_block = other.m_heap_block;
        other.m_heap_block = nullptr;
    }

    other.m_byte_count = sizeof('\0');
    zero_memory(other.m_inline_buffer, inline_capacity);

    return *this;
}

String& String::operator=(StringView view)
{
    clear();
    m_byte_count = view.byte_count() + sizeof('\0');

    WOBytes destination = m_inline_buffer;
    if (is_stored_on_heap()) {
        m_heap_block = String::allocate_memory(m_byte_count);
        m_heap_block->reference_count = 1;
        destination = m_heap_block->buffer;
    }

    copy_memory(destination, view.bytes(), view.byte_count());
    destination[view.byte_count()] = '\0';
    return *this;
}

void String::clear()
{
    if (is_stored_on_heap()) {
        m_heap_block->reference_count--;
        if (m_heap_block->reference_count == 0)
            String::free_memory(m_heap_block, m_byte_count);
        m_heap_block = nullptr;
    }

    m_byte_count = sizeof('\0');
    zero_memory(m_inline_buffer, inline_capacity);
}

String::HeapBlock* String::allocate_memory(usize in_byte_count)
{
    ASSERT(in_byte_count > inline_capacity);
    usize allocation_size = sizeof(HeapBlock) + in_byte_count;
    void* memory = ::operator new(allocation_size);
    HeapBlock* block = new (memory) HeapBlock();
    return block;
}

void String::free_memory(HeapBlock* block, usize in_byte_count)
{
    ASSERT(block->reference_count == 0);
    MAYBE_UNUSED usize allocation_size = sizeof(HeapBlock) + in_byte_count;
    ::operator delete(block);
}

void append_to_builder(StringBuilder& builder, Optional<StringView>, String const& string)
{
    builder.append_utf8(string.view());
}

} // namespace AND
