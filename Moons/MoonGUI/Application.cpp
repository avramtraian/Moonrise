/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "MoonGUI/Application.h"
#include "AT/OwnPtr.h"
#include "MoonCore/Log.h"

namespace GUI {

static OwnPtr<Application> s_application_instance;

ErrorOr<void> Application::create(const Info& info)
{
    (void)info;

    if (s_application_instance.is_valid()) {
        errorln("The application instance was already initialized!"sv);
        return Error::Unknown;
    }

    s_application_instance = make_own<Application>();
    return {};
}

void Application::destroy()
{
    if (!s_application_instance.is_valid()) {
        errorln("The application instance was already destroyed!"sv);
        return;
    }

    // Release the application instance memory.
    s_application_instance.clear();
}

Application& Application::get()
{
    AT_ASSERT(s_application_instance.is_valid());
    return s_application_instance.get();
}

} // namespace GUI
