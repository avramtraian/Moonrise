/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Optional.h>
#include <AND/StringBuilder.h>
#include <Gfx/Point.h>

namespace Gfx {

void append_to_builder(StringBuilder& builder, Optional<StringView> const&, IntPoint const& point)
{
    builder.append_formatted(VIEW("({}, {})"), point.x, point.y);
}

void append_to_builder(StringBuilder& builder, Optional<StringView> const&, FloatPoint const& point)
{
    builder.append_formatted(VIEW("({}, {})"), point.x, point.y);
}

} // namespace Gfx
