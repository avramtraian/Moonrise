/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Platform/EventLoop.h>
#include <GUI/Platform/WindowsEventLoop.h>

namespace GUI {

NonnullOwnPtr<EventLoop> EventLoop::construct()
{
    OwnPtr<EventLoop> event_loop;
#if AND_PLATFORM_OS_WINDOWS
    event_loop = adopt_nonnull_own(*new WindowsEventLoop());
#endif
    return event_loop;
}

} // namespace GUI
