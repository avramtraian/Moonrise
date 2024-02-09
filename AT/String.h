/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Error.h"
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
    // NOTE: Copying & assigning strings can be done easily using the appropriate constructor
    //       and assignment operator. However, by using that method you lose the ability to
    //       report failures using ErrorOr, as those function don't return anything.
    //       Thus, the recommended way of copying strings, by constructing or assigning, is to
    //       use the following two functions, that provide error handling out of the box.
    //       We don't delete the copy constructor & assignment operator because that would really
    //       affect the ergonomics of using this container. Any class/struct that has a string
    //       as a field would also lose its default copy constructor & assignment operator, which
    //       would be very inconvenient to use.

    AT_API static ErrorOr<String> try_copy(StringView source_view);
    AT_API static ErrorOr<void> try_assign(String& self, StringView view_to_assign);

    ALWAYS_INLINE static ErrorOr<String> try_copy(const String& source_string)
    {
        auto source_string_view = source_string.view();
        TRY_ASSIGN(auto destination_string, try_copy(source_string_view));
        return destination_string;
    }

    ALWAYS_INLINE static ErrorOr<void> try_assign(String& self, const String& string_to_assign)
    {
        auto string_view_to_assign = string_to_assign.view();
        TRY(try_assign(self, string_view_to_assign));
        return {};
    }

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
    NODISCARD ALWAYS_INLINE char* characters() { return is_stored_inline() ? m_inline_buffer : m_heap_buffer; }
    NODISCARD ALWAYS_INLINE const char* characters() const
    {
        return is_stored_inline() ? m_inline_buffer : m_heap_buffer;
    }

    NODISCARD ALWAYS_INLINE char* operator*() { return characters(); }
    NODISCARD ALWAYS_INLINE const char* operator*() const { return characters(); }

    NODISCARD ALWAYS_INLINE StringView view() const
    {
        // NOTE: If the string is small enough it will be stored inline. Having a null-terminated
        //       string that occupies zero bytes makes no sense, since just storing that byte
        //       inline is extremely cheap.
        //       If this assert fails, the String implementation *contains* a bug - not the user fault.
        AT_ASSERT(m_byte_count > 0);

        // NOTE: The string view must not be null-terminated, so including the null-termination
        //       character from the string would create a lot of confusion and bugs.
        return StringView::from_utf8(characters(), m_byte_count - sizeof('\0'));
    }

    //
    // The number of bytes the string occupies, *excluding* the null-termination character.
    // Because the string is encoded as UTF-8, the number of actual codepoints the string
    // contains might be different from the number of bytes it occupied, so never assume it.
    //
    NODISCARD ALWAYS_INLINE usize byte_count() const
    {
        // NOTE: See the comment about the assertion in the view() function.
        AT_ASSERT(m_byte_count > 0);
        return m_byte_count - 1;
    }

    //
    // The number of codepoints the string contains, *excluding* the null-termination character.
    //
    NODISCARD ALWAYS_INLINE usize length() const { return view().length(); }

    NODISCARD ALWAYS_INLINE usize byte_count_with_null_termination() const { return m_byte_count; }
    NODISCARD ALWAYS_INLINE usize length_with_null_termination() const { return length() + 1; }

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