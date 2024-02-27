/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AT/Defines.h"

#ifdef CORE_EXPORT_API
    #define MOONCORE_API AT_API_SPECIFIER_EXPORT
#else
    #define MOONCORE_API AT_API_SPECIFIER_IMPORT
#endif // CORE_EXPORT_API