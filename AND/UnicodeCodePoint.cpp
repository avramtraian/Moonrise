/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/UnicodeCodePoint.h"

namespace AND {

CompareResult compare_code_points(u32 left_code_point, u32 right_code_point)
{
    if (left_code_point < right_code_point)
        return CompareResult::less();
    if (left_code_point > right_code_point)
        return CompareResult::greater();
    return CompareResult::equal();
}

CompareResult compare_code_points_ignoring_case(u32 left_code_point, u32 right_code_point)
{
    u32 lhs = to_ascii_lower(left_code_point);
    u32 rhs = to_ascii_lower(right_code_point);
    return compare_code_points(lhs, rhs);
}

u32 to_ascii_lower(u32 code_point)
{
    if ('A' <= code_point && code_point <= 'Z')
        return 'a' + (code_point - 'A');
    return code_point;
}

u32 to_ascii_upper(u32 code_point)
{
    if ('a' <= code_point && code_point <= 'z')
        return 'A' + (code_point - 'a');
    return code_point;
}

} // namespace AND
