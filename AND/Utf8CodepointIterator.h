/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Optional.h"
#include "AND/Utf8.h"

namespace AND {

//=================================================================================================
// FORWARD ITERATOR.
//=================================================================================================

class Utf8CodePointIterator {
public:
    Utf8CodePointIterator(ROBytes bytes, usize byte_count);

    Utf8CodePointIterator(Utf8CodePointIterator const&) = default;
    Utf8CodePointIterator& operator=(Utf8CodePointIterator const&) = default;

public:
    // When encountering a byte not correctly encoded as UTF-8 (either the leading byte is incorrect or
    // the continuation bytes are incorrect), we return instead the Unicode replacement character (0xFFFD).
    // Note that we return the Unicode replacement character for each byte in an invalid sequence.
    NODISCARD u32 operator*() const;

    // When the iterator points to an invalid byte, this function will return sizeof(ROByte), i.e. 1.
    NODISCARD usize code_point_byte_width() const;

    Utf8CodePointIterator& operator++();
    Utf8CodePointIterator operator++(int);

    NODISCARD bool operator==(Utf8CodePointIterator const& other) const;
    NODISCARD bool operator!=(Utf8CodePointIterator const& other) const;

public:
    NODISCARD Optional<u32> peek(u32 offset_in_code_points = 1) const;
    NODISCARD bool is_done() const;

    // Returns if the iterator points to a valid byte sequence. Dereferencing the iterator when this
    // function evaluates to false is, however, well-defined, as the Unicode replacement character
    // will be returned instead.
    NODISCARD bool code_point_is_valid() const;

private:
    ROBytes m_bytes;
    usize m_byte_count;
    // When it has value, it contains the decoded code point located at the 'm_bytes' address.
    Optional<Utf8DecodeResult> m_current_decode_result;
};

//=================================================================================================
// REVERSE ITERATOR.
//=================================================================================================

class Utf8ReverseCodePointIterator {
public:
    Utf8ReverseCodePointIterator(ROBytes bytes, usize byte_count);

    Utf8ReverseCodePointIterator(Utf8ReverseCodePointIterator const&) = default;
    Utf8ReverseCodePointIterator& operator=(Utf8ReverseCodePointIterator const&) = default;

public:
    // When encountering a byte not correctly encoded as UTF-8 (either the leading byte is incorrect or
    // the continuation bytes are incorrect), we return instead the Unicode replacement character (0xFFFD).
    // Note that we return the Unicode replacement character for each byte in an invalid sequence.
    NODISCARD u32 operator*() const;

    // When the iterator points to an invalid byte, this function will return sizeof(ROByte), i.e. 1.
    NODISCARD usize code_point_byte_width() const;

    Utf8ReverseCodePointIterator& operator++();
    Utf8ReverseCodePointIterator operator++(int);

    NODISCARD bool operator==(Utf8ReverseCodePointIterator const& other) const;
    NODISCARD bool operator!=(Utf8ReverseCodePointIterator const& other) const;

public:
    NODISCARD Optional<u32> peek(u32 offset_in_code_points = 1) const;
    NODISCARD bool is_done() const;

    // Returns if the iterator points to a valid byte sequence. Dereferencing the iterator when this
    // function evaluates to false is, however, well-defined, as the Unicode replacement character
    // will be returned instead.
    NODISCARD bool code_point_is_valid() const;

private:
    ROBytes m_bytes;
    usize m_byte_count;
    Optional<Utf8DecodeResult> m_current_decode_result;
};

} // namespace AND
