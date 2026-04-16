/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Log.h"
#include "AND/Platform.h"

namespace AND {

void dbgln_impl(Utf8View message)
{
    Platform::set_std_stream_colors(Platform::StdStream::Output, Platform::ConsoleColor::LightGray, Platform::ConsoleColor::Black);
    Platform::write_text_to_std_stream(Platform::StdStream::Output, message);
    Platform::write_text_to_std_stream(Platform::StdStream::Output, VIEW("\n"));
    Platform::flush_std_stream(Platform::StdStream::Output);
    Platform::set_std_stream_colors(Platform::StdStream::Output, {}, {});
}

void outln_impl(Utf8View message)
{
    Platform::set_std_stream_colors(Platform::StdStream::Output, Platform::ConsoleColor::LightGray, Platform::ConsoleColor::Black);
    Platform::write_text_to_std_stream(Platform::StdStream::Output, message);
    Platform::write_text_to_std_stream(Platform::StdStream::Output, VIEW("\n"));
    Platform::flush_std_stream(Platform::StdStream::Output);
    Platform::set_std_stream_colors(Platform::StdStream::Output, {}, {});
}

void warnln_impl(Utf8View message)
{
    Platform::set_std_stream_colors(Platform::StdStream::Output, Platform::ConsoleColor::LightYellow, Platform::ConsoleColor::Black);
    Platform::write_text_to_std_stream(Platform::StdStream::Output, message);
    Platform::write_text_to_std_stream(Platform::StdStream::Output, VIEW("\n"));
    Platform::flush_std_stream(Platform::StdStream::Output);
    Platform::set_std_stream_colors(Platform::StdStream::Output, {}, {});
}

void errorln_impl(Utf8View message)
{
    Platform::set_std_stream_colors(Platform::StdStream::Error, Platform::ConsoleColor::Red, Platform::ConsoleColor::Black);
    Platform::write_text_to_std_stream(Platform::StdStream::Error, message);
    Platform::write_text_to_std_stream(Platform::StdStream::Error, VIEW("\n"));
    Platform::flush_std_stream(Platform::StdStream::Error);
    Platform::set_std_stream_colors(Platform::StdStream::Error, {}, {});
}

} // namespace AND
