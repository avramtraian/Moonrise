/*
 * Copyright (c) 2024 Traian Avram. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include <AT/Types.h>

namespace Rise {

struct MainArguments {
    u32 argument_count { 0 };
    char** arguments { nullptr };
};

//
// Function that is guaranteed to be invoked on all platforms.
// Represents the platform-agnostic entry point of the application.
//
NODISCARD static i32 guarded_main(const MainArguments& arguments)
{
    (void)arguments;
    return 0;
}

} // namespace Rise

int main(int argument_count, char** arguments)
{
    Rise::MainArguments main_arguments = {};
    main_arguments.argument_count = static_cast<u32>(argument_count);
    main_arguments.arguments = arguments;

    return Rise::guarded_main(main_arguments);
}
