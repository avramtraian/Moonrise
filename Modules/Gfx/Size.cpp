/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/Optional.h>
#include <AND/StringBuilder.h>
#include <Gfx/Size.h>

namespace Gfx {

void append_to_builder(StringBuilder& builder, Optional<StringView> const&, IntSize const& size)
{
    builder.append_formatted(VIEW("{}x{}"), size.width(), size.height());
}

void append_to_builder(StringBuilder& builder, Optional<StringView> const&, FloatSize const& size)
{
    builder.append_formatted(VIEW("{}x{}"), size.width(), size.height());
}

} // namespace Gfx
