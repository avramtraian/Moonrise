/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/StringView.h"
#include "AT/Utf8.h"

namespace AT {

ErrorOr<StringView> StringView::create_from_utf8(const char* characters, usize byte_count)
{
    TRY(UTF8::try_check_validity({ reinterpret_cast<ReadonlyBytes>(characters), byte_count }));
    return unsafe_create_from_utf8(characters, byte_count);
}

ErrorOr<StringView> StringView::create_from_utf8(const char* null_terminated_characters)
{
    TRY_ASSIGN(auto byte_count, UTF8::try_byte_count(reinterpret_cast<ReadonlyBytes>(null_terminated_characters)));
    return unsafe_create_from_utf8(null_terminated_characters, byte_count);
}

} // namespace AT