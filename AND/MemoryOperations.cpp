/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/MemoryOperations.h"

namespace AND {

void copy_memory(void* destination, void const* source, usize byte_count)
{
    WOBytes dst = static_cast<WOBytes>(destination);
    ROBytes src = static_cast<ROBytes>(source);

    for (usize byte_offset = 0; byte_offset < byte_count; ++byte_offset)
        dst[byte_offset] = src[byte_offset];
}

void copy_memory_reversed(void* destination, void const* source, usize byte_count)
{
    WOBytes dst = static_cast<WOBytes>(destination);
    ROBytes src = static_cast<ROBytes>(source);

    for (ssize byte_offset = byte_count - 1; byte_offset >= 0; --byte_offset)
        dst[byte_offset] = src[byte_offset];
}

void set_memory(void* destination, RWByte byte_value, usize byte_count)
{
    WOBytes dst = static_cast<WOBytes>(destination);
    for (usize byte_offset = 0; byte_offset < byte_count; ++byte_offset)
        dst[byte_offset] = byte_value;
}

void zero_memory(void* destination, usize byte_count)
{
    WOBytes dst = static_cast<WOBytes>(destination);
    for (usize byte_offset = 0; byte_offset < byte_count; ++byte_offset)
        dst[byte_offset] = 0;
}

} // namespace AND
