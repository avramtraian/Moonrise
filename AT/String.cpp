/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/String.h"
#include "AT/MemoryOperations.h"

namespace AT {

ErrorOr<String> String::try_copy(StringView source_view)
{
    String destination_string;
    destination_string.m_byte_count = source_view.byte_count() + 1;

    if (destination_string.is_stored_inline()) {
        copy_memory(destination_string.m_inline_buffer, source_view.characters(), source_view.byte_count());
        destination_string.m_inline_buffer[destination_string.m_byte_count - 1] = '\0';
    }
    else {
        TRY_ASSIGN(destination_string.m_heap_buffer, allocate_memory(destination_string.m_byte_count));
        copy_memory(destination_string.m_heap_buffer, source_view.characters(), source_view.byte_count());
        destination_string.m_heap_buffer[destination_string.m_byte_count - 1] = '\0';
    }

    return destination_string;
}

ErrorOr<void> String::try_assign(String& self, StringView view_to_assign)
{
    if (self.is_stored_inline()) {
        if (view_to_assign.byte_count() < inline_capacity) {
            self.m_byte_count = view_to_assign.byte_count() + 1;
            zero_memory(self.m_inline_buffer, inline_capacity);
            copy_memory(self.m_inline_buffer, view_to_assign.characters(), view_to_assign.byte_count());
            self.m_inline_buffer[self.m_byte_count - 1] = 0;
        }
        else {
            self.m_byte_count = view_to_assign.byte_count() + 1;
            TRY_ASSIGN(self.m_heap_buffer, allocate_memory(self.m_byte_count));
            copy_memory(self.m_heap_buffer, view_to_assign.characters(), view_to_assign.byte_count());
            self.m_heap_buffer[self.m_byte_count - 1] = 0;
        }
    }
    else {
        if (view_to_assign.byte_count() <= inline_capacity) {
            TRY(release_memory(self.m_heap_buffer, self.m_byte_count));
            self.m_heap_buffer = nullptr;
            self.m_byte_count = view_to_assign.byte_count() + 1;
            copy_memory(self.m_inline_buffer, view_to_assign.characters(), view_to_assign.byte_count());
            self.m_inline_buffer[self.m_byte_count - 1] = 0;
        }
        else {
            if (self.m_byte_count != view_to_assign.byte_count()) {
                TRY(release_memory(self.m_heap_buffer, self.m_byte_count));
                self.m_byte_count = view_to_assign.byte_count() + 1;
                TRY_ASSIGN(self.m_heap_buffer, allocate_memory(self.m_byte_count));
            }

            copy_memory(self.m_heap_buffer, view_to_assign.characters(), view_to_assign.byte_count());
            self.m_heap_buffer[self.m_byte_count - 1] = 0;
        }
    }

    return {};
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
    : m_byte_count(other.m_byte_count)
{
    // NOTE: Currently, the exact same code is also located in try_copy(). However, wrapping the
    //       constructor around that function would create many additional move operations. These
    //       operations are not expensive, but they make the code a lot harder to follow.
    // TODO: Maybe wrap the constructor around try_copy()?

    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, other.m_inline_buffer, m_byte_count);
    }
    else {
        MUST_ASSIGN(m_heap_buffer, allocate_memory(m_byte_count));
        copy_memory(m_heap_buffer, other.m_heap_buffer, m_byte_count);
    }
}

String::String(String&& other) noexcept
    : m_heap_buffer(other.m_heap_buffer)
    , m_byte_count(other.m_byte_count)
{
    other.m_inline_buffer[0] = 0;
    other.m_byte_count = 1;
}

String::String(StringView string_view)
    : m_byte_count(string_view.byte_count() + 1)
{
    if (is_stored_inline()) {
        copy_memory(m_inline_buffer, *string_view, string_view.byte_count());
        m_inline_buffer[m_byte_count - 1] = '\0';
    }
    else {
        MUST_ASSIGN(m_heap_buffer, allocate_memory(m_byte_count));
        copy_memory(m_heap_buffer, *string_view, string_view.byte_count());
        m_heap_buffer[m_byte_count - 1] = '\0';
    }
}

String& String::operator=(const String& other)
{
    MUST(try_assign(*this, other.view()));
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
    MUST(try_assign(*this, string_view));
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