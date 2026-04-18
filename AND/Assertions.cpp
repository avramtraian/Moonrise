/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Assertions.h"
#include "AND/Log.h"
#include "AND/Platform.h"

namespace AND {

void on_assertion_failed(AssertionKind kind, char const* expression, char const* file, char const* function, int line, char const* message)
{
    // FIXME: This is a really quick-and-dirty way of logging to the console whatever information is available
    //        about the failed assert. The string formatting in particular is really messy and should probably
    //        be strengthen by implementing a proper message layout system.

    StringView expression_view = StringView::from_null_terminated(expression);
    StringView file_view = StringView::from_null_terminated(file);
    StringView function_view = StringView::from_null_terminated(function);
    StringView message_view = StringView::from_null_terminated(message ? message : "<no message>");

    errorln("\n#====================================================================================================");
    switch (kind) {
    case AssertionKind::Assert: {
        errorln("# ASSERT: {}\n#  - At:      '{}:{}'\n#  - In:      '{}'\n#  - Message: '{}'", expression_view, file_view, line, function_view, message_view);
        break;
    }
    case AssertionKind::AssertNotReached: {
        errorln("# ASSERT NOT REACHED:\n#  - At:      '{}:{}'\n#  - In:      '{}'\n#  - Message: '{}'", file_view, line, function_view, message_view);
        break;
    }
    case AssertionKind::Panic: {
        errorln("# PANIC:\n#  - At:      '{}:{}'\n#  - In:      '{}'\n#  - Message: '{}'", file_view, line, function_view, message_view);
        break;
    }
    }

    // NOTE: Skip the 'on_assertion_failed' and 'Platform::get_call_stack()' frames since they are not relevant.
    static constexpr u32 frames_to_skip = 2;
    Vector<Platform::CallStackFrame> call_stack = Platform::get_call_stack(frames_to_skip);
    if (!call_stack.is_empty()) {
        errorln("#\n# CALL STACK:");
        for (auto const& frame : call_stack) {
            auto frame_return_address = reinterpret_cast<void*>(frame.return_address);
            auto frame_module = frame.module.value_or(VIEW("<unavailable>"));
            auto frame_function = frame.function.value_or(VIEW("<unavailable>"));
            auto frame_file = frame.file.value_or(VIEW("<unavailable>"));
            auto frame_line = frame.line.has_value() ? String::number_unsigned(*frame.line) : VIEW("<unavailable>");
            errorln("#   - {} {}!{} <- {}:{}", frame_return_address, frame_module, frame_function, frame_file, frame_line);
        }
    }

    errorln("#====================================================================================================\n");
}

NORETURN void noreturn_function()
{
    while (true) { }
}

} // namespace AND
