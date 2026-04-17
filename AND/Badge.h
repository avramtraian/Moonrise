/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Noncopyable.h>
#include <AND/Types.h>

namespace AND {

template<typename T>
class Badge {
    friend T;

private:
    Badge() = default;
    Badge(Badge const&) = default;
};

} // namespace AND

#if AND_INCLUDE_IN_GLOBAL_NAMESPACE
using AND::Badge;
#endif
