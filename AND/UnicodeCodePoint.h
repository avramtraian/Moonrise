/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/CompareResult.h"
#include "AND/Types.h"

namespace AND {

NODISCARD CompareResult compare_code_points(u32 left_code_point, u32 right_code_point);
NODISCARD CompareResult compare_code_points_ignoring_case(u32 left_code_point, u32 right_code_point);

NODISCARD u32 to_ascii_lower(u32 code_point);
NODISCARD u32 to_ascii_upper(u32 code_point);

} // namespace AND
