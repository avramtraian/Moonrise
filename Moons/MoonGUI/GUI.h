/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AT/Defines.h>

#ifdef MOON_GUI_EXPORT_API
    #define GUI_API AT_API_SPECIFIER_EXPORT
#else
    #define GUI_API AT_API_SPECIFIER_IMPORT
#endif // MOON_GUI_EXPORT_API
