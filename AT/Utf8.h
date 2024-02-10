/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreDefines.h"
#include "AT/CoreTypes.h"
#include "AT/Error.h"

namespace AT {

class UTF8 {
public:
    //
    // Converts a sequence of UTF-8 encoded bytes to the corresponding Unicode codepoint.
    // If the byte sequence is not valid UTF-8, 'invalid_unicode_codepoint' will be returned and
    // the 'out_codepoint_width' parameter will be set as zero.
    //
    NODISCARD AT_API static UnicodeCodepoint
    bytes_to_codepoint(ReadonlyBytes bytes, usize byte_count, usize& out_codepoint_width);

    //
    // Function that wraps the result of bytes_to_codepoint() into an ErrorOr.
    //
    ALWAYS_INLINE static ErrorOr<UnicodeCodepoint>
    try_bytes_to_codepoint(ReadonlyBytes bytes, usize byte_count, usize& out_codepoint_width)
    {
        auto codepoint = bytes_to_codepoint(bytes, byte_count, out_codepoint_width);
        if (codepoint == invalid_unicode_codepoint) {
            return Error::from_error_code(Error::InvalidEncoding);
        }
        return codepoint;
    }

    //
    // Computes the width (in bytes) of the codepoint, encoded as UTF-8, that is represented
    // by the given UTF-8 encoded byte sequence.
    // If the byte sequence is not valid UTF-8, zero will be returned.
    //
    NODISCARD AT_API static usize bytes_to_codepoint_width(ReadonlyBytes bytes, usize byte_count);

    //
    // Functions that wraps the result of bytes_to_codepoint_width() into an ErrorOr.
    //
    NODISCARD ALWAYS_INLINE static ErrorOr<usize> try_bytes_to_codepoint_width(ReadonlyBytes bytes, usize byte_count)
    {
        auto codepoint_width = bytes_to_codepoint_width(bytes, byte_count);
        if (codepoint_width == 0) {
            return Error::from_error_code(Error::InvalidEncoding);
        }
        return codepoint_width;
    }

    //
    // Converts a Unicode codepoint to a UTF-8 encoded byte sequence.
    // If the codepoint is not valid Unicode, no memory will be written and zero will be returned.
    // If the destination buffer is not big enough, no memory will be written and zero will be returned.
    //
    NODISCARD AT_API static usize bytes_from_codepoint(
        UnicodeCodepoint codepoint, WriteonlyBytes destination_buffer, usize destination_buffer_byte_count
    );

    //
    // Functions that wraps the result of bytes_from_codepoint() into an ErrorOr.
    //
    NODISCARD ALWAYS_INLINE static ErrorOr<usize> try_bytes_from_codepoint(
        UnicodeCodepoint codepoint, WriteonlyBytes destination_buffer, usize destination_buffer_byte_count
    )
    {
        auto codepoint_width = bytes_from_codepoint(codepoint, destination_buffer, destination_buffer_byte_count);
        if (codepoint_width == 0) {
            // TODO: This can also be caused by the buffer not being big enough, so this error code
            //       might be very misleading. However, the base bytes_from_codepoint() function
            //       gives no feedback regarding which failure occurred.
            return Error::from_error_code(Error::InvalidEncoding);
        }
        return codepoint_width;
    }

    //
    // Computes the width (in bytes) of a Unicode codepoint, encoded as UTF-8.
    // If the codepoint is not valid Unicode, zero will be returned.
    //
    NODISCARD AT_API static usize codepoint_width(UnicodeCodepoint codepoint);

    //
    // Functions that wraps the result of codepoint_width() into an ErrorOr.
    //
    NODISCARD ALWAYS_INLINE static ErrorOr<usize> try_codepoint_width(UnicodeCodepoint codepoint)
    {
        auto width = codepoint_width(codepoint);
        if (width == 0) {
            return Error::from_error_code(Error::InvalidEncoding);
        }
        return width;
    }

public:
    //
    // Computes the number of codepoints that the UTF-8 encoded byte sequence contains.
    // The length will include any character and the function will not stop if it encounters a null-termination
    // character. If the byte sequence is not valid UTF-8, 'invalid_size' will be returned.
    //
    NODISCARD AT_API static usize length(ReadonlyBytes bytes, usize byte_count);

    //
    // Functions that wraps the result of length() into an ErrorOr.
    //
    NODISCARD ALWAYS_INLINE static ErrorOr<usize> try_length(ReadonlyBytes bytes, usize byte_count)
    {
        auto len = length(bytes, byte_count);
        if (len == invalid_size) {
            return Error::from_error_code(Error::InvalidEncoding);
        }
        return len;
    }

    //
    // Determines the number of bytes that a null-terminated UTF-8 string occupies.
    // If 'bytes' is nullptr, zero will be returned.
    // If the provided byte sequence is not valid UTF-8 'invalid_size' will be returned.
    //
    NODISCARD AT_API static usize byte_count(ReadonlyBytes bytes);

    //
    // Functions that wraps the result of byte_count() into an ErrorOr.
    //
    NODISCARD ALWAYS_INLINE static ErrorOr<usize> try_byte_count(ReadonlyBytes bytes)
    {
        auto count = byte_count(bytes);
        if (count == invalid_size) {
            return Error::from_error_code(Error::InvalidEncoding);
        }
        return count;
    }

    NODISCARD AT_API static bool check_validity(ReadonlyBytes bytes, usize byte_count);

    ALWAYS_INLINE static ErrorOr<void>
    try_check_validity(ReadonlyBytes bytes, usize byte_count, Error::Code error_code = Error::InvalidEncoding)
    {
        if (!check_validity(bytes, byte_count)) {
            return Error::from_error_code(error_code);
        }
        return {};
    }
};

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::UTF8;
#endif // AT_INCLUDE_GLOBALLY