/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AND/StringBuilder.h>
#include <GUI/Length.h>

namespace GUI {

void append_to_builder(StringBuilder& builder, Optional<StringView> const&, Length const& length)
{
    if (length.is_pixels())
        builder.append_formatted(VIEW("{}px"), length.pixels());
    else
        builder.append_formatted(VIEW("{}%"), 100.0F * length.percentage());
}

} // namespace GUI
