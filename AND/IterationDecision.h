/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Types.h"

namespace AND {

enum class IterationDecision : u8 {
    Continue,
    Break,
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::IterationDecision;
#endif
