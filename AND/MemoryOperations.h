/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

void copy_memory(void* destination, void const* source, usize byte_count);

void copy_memory_reversed(void* destination, void const* source, usize byte_count);

void set_memory(void* destination, RWByte byte_value, usize byte_count);

void zero_memory(void* destination, usize byte_count);

} // namespace AND
