/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Utf8.h"

namespace AND {

// https://www.ietf.org/rfc/rfc3629.txt
// +---------------------+-------------------------------------+
// | Char. number range  |         UTF-8 octet sequence        |
// |    (hexadecimal)    |               (binary)              |
// +---------------------+-------------------------------------+
// | 0000 0000-0000 007F | 0xxxxxxx                            |
// | 0000 0080-0000 07FF | 110xxxxx 10xxxxxx                   |
// | 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx          |
// | 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx |
// +---------------------+-------------------------------------+

#define AND_UTF8_CHECK_FOR_ONE_BYTE_SEQUENCE(byte) \
    (((byte) & 0b1'0000000) == 0b0'0000000)

#define AND_UTF8_CHECK_FOR_TWO_BYTES_SEQUENCE(byte) \
    (((byte) & 0b111'00000) == 0b110'00000)

#define AND_UTF8_CHECK_FOR_THREE_BYTES_SEQUENCE(byte) \
    (((byte) & 0b1111'0000) == 0b1110'0000)

#define AND_UTF8_CHECK_FOR_FOUR_BYTES_SEQUENCE(byte) \
    (((byte) & 0b11111'000) == 0b11110'000)

#define AND_UTF8_VALIDATE_CONTINUATION_BYTE(byte) \
    if (((byte) & 0b11'000000) != 0b10'000000) {  \
        return {};                                \
    }

#define AND_UTF8_VALIDATE_ENOUGH_BYTES(byte_count, minimum_expected) \
    if ((byte_count) < (minimum_expected)) {                         \
        return {};                                                   \
    }

NODISCARD ALWAYS_INLINE static Optional<Utf8DecodeResult> utf8_decode_one_byte(
    ROByte byte_0)
{
    // NOTE: The encoded sequence: 0xxxxxxx.

    Utf8DecodeResult decode_result = {};
    decode_result.code_point |= (byte_0 & 0b0'1111111) << 0;

    // The decoded code point must be in the specification range.
    if (decode_result.code_point < 0x0000'0000 || decode_result.code_point > 0x0000'007F)
        return {};

    decode_result.byte_width = 1;
    return decode_result;
}

NODISCARD ALWAYS_INLINE static Optional<Utf8DecodeResult> utf8_decode_two_bytes(
    ROByte byte_0,
    ROByte byte_1)
{
    // NOTE: The encoded sequence: 110xxxxx 10xxxxxx.

    Utf8DecodeResult decode_result = {};
    decode_result.code_point |= (byte_0 & 0b000'11111) << 6;
    decode_result.code_point |= (byte_1 & 0b00'111111) << 0;

    // The decoded code point must be in the specification range.
    if (decode_result.code_point < 0x0000'0080 || decode_result.code_point > 0x0000'07FF)
        return {};

    decode_result.byte_width = 2;
    return decode_result;
}

NODISCARD ALWAYS_INLINE static Optional<Utf8DecodeResult> utf8_decode_three_bytes(
    ROByte byte_0,
    ROByte byte_1,
    ROByte byte_2)
{
    // NOTE: The encoded sequence: 1110xxxx 10xxxxxx 10xxxxxx.

    Utf8DecodeResult decode_result = {};
    decode_result.code_point |= (byte_0 & 0b0000'1111) << 12;
    decode_result.code_point |= (byte_1 & 0b00'111111) << 6;
    decode_result.code_point |= (byte_2 & 0b00'111111) << 0;

    // The decoded code point must be in the specification range.
    if (decode_result.code_point < 0x0000'0800 || decode_result.code_point > 0x0000'FFFF)
        return {};

    // https://www.ietf.org/rfc/rfc3629.txt
    // The definition of UTF-8 prohibits encoding character numbers between U+D800 and U+DFFF, which are reserved
    // for use with the UTF-16 encoding form (as surrogate pairs) and do not directly represent characters.
    if (0xD800 <= decode_result.code_point && decode_result.code_point <= 0xDFFF)
        return {};

    decode_result.byte_width = 3;
    return decode_result;
}

NODISCARD ALWAYS_INLINE static Optional<Utf8DecodeResult> utf8_decode_four_bytes(
    ROByte byte_0,
    ROByte byte_1,
    ROByte byte_2,
    ROByte byte_3)
{
    // NOTE: The encoded sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx.

    Utf8DecodeResult decode_result = {};
    decode_result.code_point |= (byte_0 & 0b00000'111) << 18;
    decode_result.code_point |= (byte_1 & 0b00'111111) << 12;
    decode_result.code_point |= (byte_2 & 0b00'111111) << 6;
    decode_result.code_point |= (byte_3 & 0b00'111111) << 0;

    // The decoded code point must be in the specification range.
    if (decode_result.code_point < 0x0001'0000 || decode_result.code_point > 0x0010'FFFF)
        return {};

    decode_result.byte_width = 4;
    return decode_result;
}

Optional<Utf8DecodeResult> utf8_decode(void const* data, usize size)
{

    AND_UTF8_VALIDATE_ENOUGH_BYTES(size, 1);
    ROBytes bytes = static_cast<ROBytes>(data);
    ROByte first_byte = bytes[0];

    if (AND_UTF8_CHECK_FOR_ONE_BYTE_SEQUENCE(first_byte)) {
        AND_UTF8_VALIDATE_ENOUGH_BYTES(size, 1);
        return utf8_decode_one_byte(bytes[0]);
    }

    if (AND_UTF8_CHECK_FOR_TWO_BYTES_SEQUENCE(first_byte)) {
        AND_UTF8_VALIDATE_ENOUGH_BYTES(size, 2);
        AND_UTF8_VALIDATE_CONTINUATION_BYTE(bytes[1]);
        return utf8_decode_two_bytes(bytes[0], bytes[1]);
    }

    if (AND_UTF8_CHECK_FOR_THREE_BYTES_SEQUENCE(first_byte)) {
        AND_UTF8_VALIDATE_ENOUGH_BYTES(size, 3);
        AND_UTF8_VALIDATE_CONTINUATION_BYTE(bytes[1]);
        AND_UTF8_VALIDATE_CONTINUATION_BYTE(bytes[2]);
        return utf8_decode_three_bytes(bytes[0], bytes[1], bytes[2]);
    }

    if (AND_UTF8_CHECK_FOR_FOUR_BYTES_SEQUENCE(first_byte)) {
        AND_UTF8_VALIDATE_ENOUGH_BYTES(size, 4);
        AND_UTF8_VALIDATE_CONTINUATION_BYTE(bytes[1]);
        AND_UTF8_VALIDATE_CONTINUATION_BYTE(bytes[2]);
        AND_UTF8_VALIDATE_CONTINUATION_BYTE(bytes[3]);
        return utf8_decode_four_bytes(bytes[0], bytes[1], bytes[2], bytes[3]);
    }

    // The first byte is not correctly encoded as UTF-8.
    return {};
}

Optional<Utf8DecodeResult> utf8_decode_reversed(void const* data, usize size)
{
    AND_UTF8_VALIDATE_ENOUGH_BYTES(size, 1);
    ROBytes bytes = static_cast<ROBytes>(data) + size;

    if (AND_UTF8_CHECK_FOR_ONE_BYTE_SEQUENCE(bytes[-1])) {
        return utf8_decode_one_byte(bytes[-1]);
    }

    AND_UTF8_VALIDATE_CONTINUATION_BYTE(bytes[-1]);
    AND_UTF8_VALIDATE_ENOUGH_BYTES(size, 2);

    if (AND_UTF8_CHECK_FOR_TWO_BYTES_SEQUENCE(bytes[-2])) {
        return utf8_decode_two_bytes(bytes[-2], bytes[-1]);
    }

    AND_UTF8_VALIDATE_CONTINUATION_BYTE(bytes[-2]);
    AND_UTF8_VALIDATE_ENOUGH_BYTES(size, 3);

    if (AND_UTF8_CHECK_FOR_THREE_BYTES_SEQUENCE(bytes[-3])) {
        return utf8_decode_three_bytes(bytes[-3], bytes[-2], bytes[-1]);
    }

    AND_UTF8_VALIDATE_CONTINUATION_BYTE(bytes[-3]);
    AND_UTF8_VALIDATE_ENOUGH_BYTES(size, 4);

    if (AND_UTF8_CHECK_FOR_FOUR_BYTES_SEQUENCE(bytes[-4])) {
        return utf8_decode_four_bytes(bytes[-4], bytes[-3], bytes[-2], bytes[-1]);
    }

    return {};
}

Optional<Utf8EncodeResult> utf8_encode(u32 code_point)
{
    // https://www.ietf.org/rfc/rfc3629.txt
    // +---------------------+-------------------------------------+
    // | Char. number range  |         UTF-8 octet sequence        |
    // |    (hexadecimal)    |               (binary)              |
    // +---------------------+-------------------------------------+
    // | 0000 0000-0000 007F | 0xxxxxxx                            |
    // | 0000 0080-0000 07FF | 110xxxxx 10xxxxxx                   |
    // | 0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx          |
    // | 0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx |
    // +---------------------+-------------------------------------+

    if (0x0000'0000 <= code_point && code_point <= 0x0000'007F) {
        // The rune needs 1-byte to be encoded: 0xxxxxxx.
        Utf8EncodeResult encode_result;
        encode_result.byte_width = 1;
        encode_result.encoded[0] = 0b0'0000000 | ((code_point >> 0) & 0b0'1111111);
        return encode_result;
    }

    if (0x0000'0080 <= code_point && code_point <= 0x0000'07FF) {
        // The rune needs 2-bytes to be encoded: 110xxxxx 10xxxxxx.
        Utf8EncodeResult encode_result;
        encode_result.byte_width = 2;
        encode_result.encoded[0] = 0b110'00000 | ((code_point >> 6) & 0b000'11111);
        encode_result.encoded[1] = 0b10'000000 | ((code_point >> 0) & 0b00'111111);
        return encode_result;
    }

    if (0x0000'0800 <= code_point && code_point <= 0x0000'FFFF) {
        // https://www.ietf.org/rfc/rfc3629.txt
        // The definition of UTF-8 prohibits encoding character numbers between U+D800 and U+DFFF, which are reserved
        // for use with the UTF-16 encoding form (as surrogate pairs) and do not directly represent characters.
        if (0xD800 <= code_point && code_point <= 0xDFFF)
            return {};

        // The rune needs 3-bytes to be encoded: 1110xxxx 10xxxxxx 10xxxxxx.
        Utf8EncodeResult encode_result;
        encode_result.byte_width = 3;
        encode_result.encoded[0] = 0b1110'0000 | ((code_point >> 12) & 0b0000'1111);
        encode_result.encoded[1] = 0b10'000000 | ((code_point >> 6) & 0b00'111111);
        encode_result.encoded[2] = 0b10'000000 | ((code_point >> 0) & 0b00'111111);
        return encode_result;
    }

    if (0x0001'0000 <= code_point && code_point <= 0x0010'FFFF) {
        // The rune needs 4-bytes to be encoded: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx.
        Utf8EncodeResult encode_result;
        encode_result.byte_width = 4;
        encode_result.encoded[0] = 0b11110'000 | ((code_point >> 18) & 0b00000'111);
        encode_result.encoded[1] = 0b10'000000 | ((code_point >> 12) & 0b00'111111);
        encode_result.encoded[2] = 0b10'000000 | ((code_point >> 6) & 0b00'111111);
        encode_result.encoded[3] = 0b10'000000 | ((code_point >> 0) & 0b00'111111);
        return encode_result;
    }

    return {};
}

} // namespace AND
