/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Types.h"

#define AT_DEFINE_BOOLEAN_ENUM(enum_name) \
    enum class enum_name : ::AT::u8 {     \
        No = 0,                           \
        Yes = 1,                          \
    };
