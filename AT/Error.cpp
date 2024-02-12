/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/Error.h"

namespace AT {

Error::Error(Error&& other) noexcept
    : m_error_kind(other.m_error_kind)
{
    switch (m_error_kind) {
        case Kind::Code: {
            m_error.code = other.m_error.code;
            other.m_error.code = Unknown;
            break;
        }

        case Kind::StringLiteral: {
            m_error.string_literal = other.m_error.string_literal;
            other.m_error.string_literal = nullptr;
            break;
        }

        case Kind::StringAllocated: {
            m_error.string_allocated.heap_buffer = other.m_error.string_allocated.heap_buffer;
            m_error.string_allocated.byte_count = other.m_error.string_allocated.byte_count;
            other.m_error.string_allocated.heap_buffer = nullptr;
            other.m_error.string_allocated.byte_count = 0;
            break;
        }
    }
}

Error Error::from_error_code(Code error_code)
{
    Error error;
    error.m_error_kind = Kind::Code;
    error.m_error.code = error_code;
    return error;
}

} // namespace AT