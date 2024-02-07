/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AT/Assertion.h"

namespace AT {

void assertion_failed(const char* expression, const char* filename, const char* function, u32 line_number)
{
    (void)expression;
    (void)filename;
    (void)function;
    (void)line_number;
}

} // namespace AT
