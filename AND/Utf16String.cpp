/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Utf16String.h"
#include "AND/MemoryOperations.h"
#include "AND/StringBuilder.h"

namespace AND {

Utf16String Utf16String::from_utf8(Utf8View const& view)
{
    StringBuilder builder;
    builder.set_encoding(StringBuilder::Encoding::UTF16);
    builder.append_utf8(view);
    return builder.build_utf16();
}

Utf16String::Utf16String()
{
    zero_memory(m_inline_buffer, inline_capacity * sizeof(u16));
    m_code_unit_count = 1;
}

Utf16String::~Utf16String()
{
    clear();
}

Utf16String::Utf16String(Utf16String const& other)
    : m_code_unit_count(other.m_code_unit_count)
{
    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_code_unit_count * sizeof(u16));
    } else {
        m_heap_block = other.m_heap_block;
        m_heap_block->reference_count++;
    }
}

Utf16String::Utf16String(Utf16String&& other) noexcept
    : m_code_unit_count(other.m_code_unit_count)
{
    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_code_unit_count * sizeof(u16));
    } else {
        m_heap_block = other.m_heap_block;
        other.m_heap_block = nullptr;
    }

    zero_memory(other.m_inline_buffer, inline_capacity * sizeof(u16));
    other.m_code_unit_count = 1;
}

Utf16String::Utf16String(Utf16View in_view)
    : m_code_unit_count(in_view.code_unit_count() + 1)
{
    u16* destination = m_inline_buffer;
    if (is_stored_on_heap()) {
        m_heap_block = Utf16String::allocate_memory(m_code_unit_count);
        m_heap_block->reference_count = 1;
        destination = m_heap_block->code_units;
    }

    copy_memory(destination, in_view.code_units(), in_view.code_unit_count() * sizeof(u16));
    destination[in_view.code_unit_count()] = '\0';
}

Utf16String& Utf16String::operator=(Utf16String const& other)
{
    // Handle the self-assignment case.
    if (this == &other)
        return *this;

    clear();
    m_code_unit_count = other.m_code_unit_count;
    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_code_unit_count * sizeof(u16));
    } else {
        m_heap_block = other.m_heap_block;
        m_heap_block->reference_count++;
    }

    return *this;
}

Utf16String& Utf16String::operator=(Utf16String&& other) noexcept
{
    // Handle the self-assignment case.
    if (this == &other)
        return *this;

    clear();
    m_code_unit_count = other.m_code_unit_count;
    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_code_unit_count * sizeof(u16));
    } else {
        m_heap_block = other.m_heap_block;
        other.m_heap_block = nullptr;
    }

    zero_memory(other.m_inline_buffer, inline_capacity * sizeof(u16));
    other.m_code_unit_count = 1;

    return *this;
}

Utf16String& Utf16String::operator=(Utf16View in_view)
{
    clear();
    m_code_unit_count = in_view.code_unit_count() + 1;

    u16* destination = m_inline_buffer;
    if (is_stored_on_heap()) {
        m_heap_block = Utf16String::allocate_memory(m_code_unit_count);
        m_heap_block->reference_count = 1;
        destination = m_heap_block->code_units;
    }

    copy_memory(destination, in_view.code_units(), in_view.code_unit_count() * sizeof(u16));
    destination[in_view.code_unit_count()] = '\0';
    return *this;
}

void Utf16String::clear()
{
    if (is_stored_on_heap()) {
        m_heap_block->reference_count--;
        if (m_heap_block->reference_count == 0)
            Utf16String::free_memory(m_heap_block, m_code_unit_count);
        m_heap_block = nullptr;
    }

    zero_memory(m_inline_buffer, inline_capacity * sizeof(u16));
    m_code_unit_count = 1;
}

Utf16String::HeapBlock* Utf16String::allocate_memory(usize in_code_unit_count)
{
    usize allocation_size = sizeof(HeapBlock) + (in_code_unit_count * sizeof(u16));
    void* memory = ::operator new(allocation_size);
    HeapBlock* block = new (memory) HeapBlock();
    return block;
}

void Utf16String::free_memory(HeapBlock* block, usize in_code_unit_count)
{
    MAYBE_UNUSED usize allocation_size = sizeof(HeapBlock) + (in_code_unit_count * sizeof(u16));
    ::operator delete(block);
}

} // namespace AND
