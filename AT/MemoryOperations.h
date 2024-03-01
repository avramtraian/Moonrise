/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Defines.h"
#include "AT/Span.h"
#include "AT/Types.h"

namespace AT {

AT_API void copy_memory(void* destination_buffer, const void* source_buffer, usize byte_count);

AT_API void set_memory(void* destination_buffer, u8 value, usize byte_count);

AT_API void zero_memory(void* destination_buffer, usize byte_count);

template<typename T>
ALWAYS_INLINE static void copy_memory_from_span(void* destination, Span<T> span)
{
    copy_memory(destination, span.elements(), span.count() * span.element_size());
}

} // namespace AT

AT_API void* operator new(std::size_t byte_count, void* memory_block) noexcept;

#ifdef AT_INCLUDE_GLOBALLY
using AT::copy_memory;
using AT::set_memory;
using AT::zero_memory;
#endif // AT_INCLUDE_GLOBALLY