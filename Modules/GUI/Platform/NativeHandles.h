/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <AND/Types.h>

namespace GUI {

using NativeWindowHandle = u32;
using NativeTimerHandle = u32;

static constexpr NativeWindowHandle invalid_native_window_handle = 0;
static constexpr NativeTimerHandle invalid_native_timer_handle = 0;

} // namespace GUI
