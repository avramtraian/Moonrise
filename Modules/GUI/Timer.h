/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include <GUI/Platform/NativeHandles.h>

namespace GUI {

class Timer {
    AND_MAKE_NONCOPYABLE(Timer);
    AND_MAKE_NONMOVABLE(Timer);

public:
    Timer();
    ~Timer();

    void initialize(TimeDuration, Function<void()>);

private:
    NativeTimerHandle m_native_handle;
};

} // namespace GUI
