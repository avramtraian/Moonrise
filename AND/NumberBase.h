/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

enum class NumberBase : u8 {
    Binary = 2,
    Octal = 8,
    Decimal = 10,
    Hexadecimal = 16,
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::NumberBase;
#endif
