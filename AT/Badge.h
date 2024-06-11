/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Types.h>

namespace AT {

template<typename T>
class Badge {
    AT_MAKE_NONCOPYABLE(Badge);
    AT_MAKE_NONMOVABLE(Badge);

    friend T;

private:
    // NOTE: Only T can access this constructor, thus only T can create this badge.
    constexpr Badge() = default;
};

} // namespace AT

#ifdef AT_INCLUDE_GLOBALLY
using AT::Badge;
#endif // AT_INCLUDE_GLOBALLY
