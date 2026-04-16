/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

enum class MatchResult : u8 {
    No = 0,
    Yes = 1,
};

NODISCARD ALWAYS_INLINE constexpr MatchResult match_result(bool value)
{
    if (value)
        return MatchResult::Yes;
    return MatchResult::No;
}

} // namespace AND
