/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Optional.h"
#include "AND/Types.h"

namespace AND {

struct Utf16DecodeResult {
    u32 code_point;
    usize byte_count;
    usize code_unit_count;
};

NODISCARD Optional<Utf16DecodeResult> utf16_decode(u16 const* code_units, usize code_unit_count);
NODISCARD Optional<Utf16DecodeResult> utf16_decode_le(void const* data, usize size);
NODISCARD Optional<Utf16DecodeResult> utf16_decode_be(void const* data, usize size);
NODISCARD Optional<Utf16DecodeResult> utf16_decode_reversed(u16 const* code_units, usize code_unit_count);
NODISCARD Optional<Utf16DecodeResult> utf16_decode_reversed_le(void const* data, usize size);
NODISCARD Optional<Utf16DecodeResult> utf16_decode_reversed_be(void const* data, usize size);

struct Utf16EncodeCodeUnitsResult {
    static constexpr usize max_code_unit_count = 2;
    usize code_unit_count;
    u16 code_units[max_code_unit_count];
};

struct Utf16EncodeBytesResult {
    static constexpr usize max_byte_count = 4;
    usize byte_count;
    RWByte bytes[max_byte_count];
};

NODISCARD Optional<Utf16EncodeCodeUnitsResult> utf16_encode(u32 code_point);
NODISCARD Optional<Utf16EncodeBytesResult> utf16_encode_le(u32 code_point);
NODISCARD Optional<Utf16EncodeBytesResult> utf16_encode_be(u32 code_point);

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::utf16_decode;
using AND::utf16_decode_be;
using AND::utf16_decode_le;
using AND::utf16_decode_reversed;
using AND::utf16_decode_reversed_be;
using AND::utf16_decode_reversed_le;
using AND::utf16_encode;
using AND::utf16_encode_be;
using AND::utf16_encode_le;
#endif
