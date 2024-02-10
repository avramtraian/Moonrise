/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/String.h"
#include "AT/MemoryOperations.h"
#include "AT/Utf8.h"

namespace AT {

ErrorOr<String> String::create(StringView view)
{
    String string;
    string.m_byte_count = view.byte_span().count() + sizeof('\0');

    if (string.is_stored_inline()) {
        copy_memory_from_span(string.m_inline_buffer, view.byte_span());
        string.m_inline_buffer[string.m_byte_count - 1] = '\0';
    }
    else {
        TRY_ASSIGN(string.m_heap_buffer, allocate_memory(string.m_byte_count));
        copy_memory_from_span(string.m_heap_buffer, view.byte_span());
        string.m_heap_buffer[string.m_byte_count - 1] = '\0';
    }

    return string;
}

ErrorOr<String> String::create(const String& other)
{
    TRY_ASSIGN(String string, create(other.view()));
    return string;
}

ErrorOr<String> String::create_from_utf8(const char* characters, usize byte_count)
{
    TRY_ASSIGN(auto view, StringView::create_from_utf8(characters, byte_count));
    TRY_ASSIGN(auto string, create(view));
    return string;
}

ErrorOr<String> String::create_from_utf8(const char* null_terminated_characters)
{
    TRY_ASSIGN(auto view, StringView::create_from_utf8(null_terminated_characters));
    TRY_ASSIGN(auto string, create(view));
    return string;
}

String::String()
{
    m_inline_buffer[0] = 0;
    m_byte_count = 1;
}

String::~String()
{
    if (is_stored_on_heap()) {
        MUST(release_memory(m_heap_buffer, m_byte_count));
    }
}

String::String(const String& other)
{
    MUST_ASSIGN(String string, create(other));
    new (this) String(move(string));
}

String::String(String&& other) noexcept
    : m_heap_buffer(other.m_heap_buffer)
    , m_byte_count(other.m_byte_count)
{
    other.m_inline_buffer[0] = 0;
    other.m_byte_count = 1;
}

String::String(StringView string_view)
{
    MUST_ASSIGN(String string, create(string_view));
    new (this) String(move(string));
}

String& String::operator=(const String& other)
{
    MUST_ASSIGN(*this, create(other));
    return *this;
}

String& String::operator=(String&& other) noexcept
{
    if (is_stored_on_heap()) {
        MUST(release_memory(m_heap_buffer, m_byte_count));
    }

    m_heap_buffer = other.m_heap_buffer;
    m_byte_count = other.m_byte_count;

    zero_memory(other.m_inline_buffer, inline_capacity);
    other.m_inline_buffer[0] = 0;
    other.m_byte_count = 1;

    return *this;
}

String& String::operator=(StringView string_view)
{
    MUST_ASSIGN(*this, create(string_view));
    return *this;
}

ErrorOr<char*> String::allocate_memory(usize byte_count)
{
    void* memory_block = ::operator new(byte_count);

    if (memory_block == nullptr) {
        return Error::from_error_code(Error::OutOfMemory);
    }
    return reinterpret_cast<char*>(memory_block);
}

ErrorOr<void> String::release_memory(char* heap_buffer, usize byte_count)
{
    // NOTE: The standard operator delete doesn't need the capacity of the memory block.
    //       However, in future implementations we might switch to a custom memory allocator,
    //       so having this crucial information available out-of-the-box is really handy.
    (void)byte_count;

    ::operator delete(heap_buffer);
    return {};
}

} // namespace AT