/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Utf16.h"
#include "AND/Endian.h"

namespace AND {

Optional<Utf16DecodeResult> utf16_decode(u16 const* code_units, usize code_unit_count)
{
    if (code_unit_count == 0)
        return {};

    if (code_units[0] < 0xD800 || 0xDFFF < code_units[0]) {
        Utf16DecodeResult decode_result;
        decode_result.code_point = code_units[0];
        decode_result.code_unit_count = 1;
        decode_result.byte_count = 2;
        return decode_result;
    }

    if (0xD800 <= code_units[0] && code_units[0] <= 0xDBFF) {
        // Check that the provided encoded buffer is large enough
        // to contain correctly encoded UTF-16 data.
        if (code_unit_count < 2)
            return {};

        // Check that the second code unit is in the valid range.
        if (code_units[1] < 0xDC00 || code_units[1] > 0xDFFF)
            return {};

        Utf16DecodeResult decode_result;
        decode_result.code_point = 0x10000 + ((code_units[0] - 0xD800) << 10) + (code_units[1] - 0xDC00);
        decode_result.code_unit_count = 2;
        decode_result.byte_count = 4;
        return decode_result;
    }

    // The first code unit is invalid.
    return {};
}

Optional<Utf16DecodeResult> utf16_decode_le(void const* data, usize size)
{
    u16 code_units[2];
    usize code_unit_count = 0;
    u16 const* code_units_big = static_cast<u16 const*>(data);

    if (2 <= size && size < 4) {
        code_units[0] = little_to_host(code_units_big[0]);
        code_unit_count = 1;
    } else if (size >= 4) {
        code_units[0] = little_to_host(code_units_big[0]);
        code_units[1] = little_to_host(code_units_big[1]);
        code_unit_count = 2;
    }

    return utf16_decode(code_units, code_unit_count);
}

Optional<Utf16DecodeResult> utf16_decode_be(void const* data, usize size)
{
    u16 code_units[2];
    usize code_unit_count = 0;
    u16 const* code_units_big = static_cast<u16 const*>(data);

    if (2 <= size && size < 4) {
        code_units[0] = big_to_host(code_units_big[0]);
        code_unit_count = 1;
    } else if (size >= 4) {
        code_units[0] = big_to_host(code_units_big[0]);
        code_units[1] = big_to_host(code_units_big[1]);
        code_unit_count = 2;
    }

    return utf16_decode(code_units, code_unit_count);
}

Optional<Utf16DecodeResult> utf16_decode_reversed(u16 const* code_units, usize code_unit_count)
{
    if (code_unit_count == 0)
        return {};

    u16 code_unit_1 = *(code_units + code_unit_count - 1);
    if (code_unit_1 < 0xD800 || 0xDFFF < code_unit_1) {
        Utf16DecodeResult decode_result;
        decode_result.code_point = code_unit_1;
        decode_result.code_unit_count = 1;
        decode_result.byte_count = 2;
        return decode_result;
    }

    if (0xDC00 <= code_unit_1 && code_unit_1 <= 0xDFFF) {
        // Check that the provided encoded buffer is large enough
        // to contain correctly encoded UTF-16 data.
        if (code_unit_count < 2)
            return {};

        // Check that the first code unit is in the valid range.
        u16 code_unit_0 = *(code_units + code_unit_count - 2);
        if (code_unit_0 < 0xD800 || code_unit_0 > 0xDBFF)
            return {};

        Utf16DecodeResult decode_result;
        decode_result.code_point = 0x10000 + ((code_unit_0 - 0xD800) << 10) + (code_unit_1 - 0xDC00);
        decode_result.code_unit_count = 2;
        decode_result.byte_count = 4;
        return decode_result;
    }

    // The second code unit is invalid.
    return {};
}

Optional<Utf16DecodeResult> utf16_decode_reversed_le(void const* data, usize size)
{
    u16 code_units[2];
    usize code_unit_count = 0;
    u16 const* code_units_little = reinterpret_cast<u16 const*>(static_cast<ROBytes>(data) + size);

    if (2 <= size && size < 4) {
        code_units[0] = little_to_host(code_units_little[-1]);
        code_unit_count = 1;
    } else if (size >= 4) {
        code_units[0] = little_to_host(code_units_little[-2]);
        code_units[1] = little_to_host(code_units_little[-1]);
        code_unit_count = 2;
    }

    return utf16_decode_reversed(code_units, code_unit_count);
}

Optional<Utf16DecodeResult> utf16_decode_reversed_be(void const* data, usize size)
{
    u16 code_units[2];
    usize code_unit_count = 0;
    u16 const* code_units_big = reinterpret_cast<u16 const*>(static_cast<ROBytes>(data) + size);

    if (2 <= size && size < 4) {
        code_units[0] = big_to_host(code_units_big[-1]);
        code_unit_count = 1;
    } else if (size >= 4) {
        code_units[0] = big_to_host(code_units_big[-2]);
        code_units[1] = big_to_host(code_units_big[-1]);
        code_unit_count = 2;
    }

    return utf16_decode_reversed(code_units, code_unit_count);
}

Optional<Utf16EncodeCodeUnitsResult> utf16_encode(u32 code_point)
{
    if (code_point <= 0xFFFF) {
        // Check that the code point is outside the surrogate range, case in which the
        // provided code point is not valid Unicode.
        if (0xD800 <= code_point && code_point <= 0xDFFF)
            return {};

        Utf16EncodeCodeUnitsResult encode_result;
        encode_result.code_unit_count = 1;
        encode_result.code_units[0] = static_cast<u16>(code_point);
        return encode_result;
    }

    if (0x10000 <= code_point && code_point <= 0x10FFFF) {
        Utf16EncodeCodeUnitsResult encode_result;
        encode_result.code_unit_count = 2;
        encode_result.code_units[0] = static_cast<u16>(((code_point - 0x10000) >> 10) | 0xD800);
        encode_result.code_units[1] = static_cast<u16>(((code_point - 0x10000) & 0x3FF) | 0xDC00);
        return encode_result;
    }

    return {};
}

Optional<Utf16EncodeBytesResult> utf16_encode_le(u32 code_point)
{
    auto encode_optional = utf16_encode(code_point);
    if (!encode_optional.has_value())
        return {};
    auto encode = encode_optional.value();

    Utf16EncodeBytesResult result;
    result.byte_count = encode.code_unit_count * 2;
    u16* dst_code_units = reinterpret_cast<u16*>(result.bytes);
    dst_code_units[0] = host_to_little(encode.code_units[0]);
    dst_code_units[1] = host_to_little(encode.code_units[1]);
    return result;
}

Optional<Utf16EncodeBytesResult> utf16_encode_be(u32 code_point)
{
    auto encode_optional = utf16_encode(code_point);
    if (!encode_optional.has_value())
        return {};
    auto encode = encode_optional.value();

    Utf16EncodeBytesResult result;
    result.byte_count = encode.code_unit_count * 2;
    u16* dst_code_units = reinterpret_cast<u16*>(result.bytes);
    dst_code_units[0] = host_to_big(encode.code_units[0]);
    dst_code_units[1] = host_to_big(encode.code_units[1]);
    return result;
}

} // namespace AND
