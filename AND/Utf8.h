/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Optional.h"
#include "AND/Types.h"

namespace AND {

struct Utf8DecodeResult {
    u32 code_point;
    usize byte_width;
};

struct Utf8EncodeResult {
    usize byte_width;
    static constexpr usize max_encoded_byte_width = 4;
    u8 encoded[max_encoded_byte_width];
};

Optional<Utf8DecodeResult> utf8_decode(void const* data, usize size);

Optional<Utf8DecodeResult> utf8_decode_reversed(void const* data, usize size);

Optional<Utf8EncodeResult> utf8_encode(u32 code_point);

} // namespace AND
