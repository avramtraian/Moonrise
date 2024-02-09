/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/StringView.h"
#include "AT/Utf8.h"

namespace AT {

usize StringView::length() const
{
    return UTF8::length(reinterpret_cast<ReadonlyBytes>(m_characters), m_byte_count);
}

} // namespace AT