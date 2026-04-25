/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Optional.h>
#include <AND/StringBuilder.h>
#include <Gfx/Rect.h>

namespace Gfx {

void append_to_builder(StringBuilder& builder, Optional<StringView> const&, IntRect const& rect)
{
    builder.append_formatted(VIEW("[{} : {}]"), rect.offset(), rect.size());
}

void append_to_builder(StringBuilder& builder, Optional<StringView> const&, FloatRect const& rect)
{
    builder.append_formatted(VIEW("[{} : {}]"), rect.offset(), rect.size());
}

} // namespace Gfx
