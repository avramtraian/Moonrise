/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <GUI/Platform/WindowManager.h>
#include <GUI/Platform/WindowsWindowManager.h>

namespace GUI {

static WindowManager* s_the_window_manager = nullptr;

void WindowManager::initialize()
{
    ASSERT(!s_the_window_manager);

#if AND_PLATFORM_OS_WINDOWS
    s_the_window_manager = new WindowsWindowManager();
#endif

    ASSERT(s_the_window_manager);
    s_the_window_manager->initialize_impl();
}

void WindowManager::shutdown()
{
    ASSERT(s_the_window_manager);
    s_the_window_manager->shutdown_impl();
    delete s_the_window_manager;
    s_the_window_manager = nullptr;
}

WindowManager& WindowManager::the()
{
    ASSERT(s_the_window_manager);
    return *s_the_window_manager;
}

} // namespace GUI
