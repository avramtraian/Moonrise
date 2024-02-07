/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/CoreDefines.h"
#include "AT/CoreTypes.h"

namespace AT {

AT_API void copy_memory(void* destination_buffer, const void* source_buffer, usize byte_count);

AT_API void set_memory(void* destination_buffer, u8 value, usize byte_count);

AT_API void zero_memory(void* destination_buffer, usize byte_count);

} // namespace AT

void* operator new(size_t byte_count, void* memory_block);