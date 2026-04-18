/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#pragma once

#include "AND/Optional.h"
#include "AND/Span.h"
#include "AND/String.h"
#include "AND/Vector.h"

namespace AND::Platform {

enum class Result {
    Success = 0,
    UnknownError = 1,
    InvalidArgument,
    NoStdStream,
    StdStreamIsNotConsole,
};

//=================================================================================================
// STANDARD I/O STREAMS.
//=================================================================================================

enum class StdStream {
    Output,
    Error,
};

enum class ConsoleColor {
    Black,
    Red,
    Green,
    Blue,
    Yellow,
    Cyan,
    Magenta,
    Gray,

    LightGray,
    LightRed,
    LightGreen,
    LightBlue,
    LightYellow,
    LightCyan,
    LightMagenta,
    White,
};

Result write_text_to_std_stream(StdStream, StringView);
Result write_binary_to_std_stream(StdStream, ROByteSpan);
Result flush_std_stream(StdStream);
Result set_std_stream_colors(StdStream, Optional<ConsoleColor> foreground, Optional<ConsoleColor> background);

//=================================================================================================
// DEBUG UTILITIES.
//=================================================================================================

struct CallStackFrame {
    uintptr return_address;
    Optional<String> module;
    Optional<String> function;
    Optional<String> file;
    Optional<u32> line;
};

Vector<CallStackFrame> get_call_stack(u32 frames_to_skip);

//=================================================================================================
// TIMING UTILITIES.
//=================================================================================================

class TimeDuration {
public:
    NODISCARD static TimeDuration from_milliseconds(f32 milliseconds);
    NODISCARD static TimeDuration from_seconds(f32 seconds);

public:
    ALWAYS_INLINE constexpr TimeDuration()
        : m_ticks(0)
    {
    }

    TimeDuration(TimeDuration const&) = default;
    TimeDuration& operator=(TimeDuration const&) = default;

    NODISCARD f32 milliseconds() const;
    NODISCARD f32 seconds() const;

private:
    ALWAYS_INLINE constexpr explicit TimeDuration(u64 ticks)
        : m_ticks(ticks)
    {
    }

private:
    u64 m_ticks;
};

void sleep(TimeDuration duration);

//=================================================================================================
// FILE.
//=================================================================================================

enum class FileError : u16 {
    Unknown = 0,
    FileNotFound,
    PermissionDenied,
    SharingViolation,
    IsDirectory,
    InvalidHandle,
    InvalidArgument,
};

template<typename T>
class FileErrorOr {
public:
    /*implicit*/ FileErrorOr(T value)
        : m_value(value)
    {
    }

    /*implicit*/ FileErrorOr(FileError error_code)
        : m_error_code(error_code)
    {
    }

    FileErrorOr(FileError error_code, StringView error_message)
        : m_error_code(error_code)
        , m_error_message(error_message)
    {
    }

public:
    NODISCARD ALWAYS_INLINE bool is_valid() const { return m_value.has_value(); }
    NODISCARD ALWAYS_INLINE bool is_error() const { return !m_value.has_value(); }
    NODISCARD ALWAYS_INLINE T& value() { return m_value.value(); }
    NODISCARD ALWAYS_INLINE T const& value() const { return m_value.value(); }
    NODISCARD ALWAYS_INLINE FileError error_code() const { return m_error_code; }
    NODISCARD ALWAYS_INLINE StringView error_message() const { return m_error_message; }

private:
    Optional<T> m_value;
    FileError m_error_code;
    StringView m_error_message;
};

template<>
class FileErrorOr<void> {
public:
    FileErrorOr()
        : m_is_error(false)
    {
    }

    /*implicit*/ FileErrorOr(FileError error_code)
        : m_is_error(false)
        , m_error_code(error_code)
    {
    }

    FileErrorOr(FileError error_code, StringView error_message)
        : m_is_error(true)
        , m_error_code(error_code)
        , m_error_message(error_message)
    {
    }

    NODISCARD ALWAYS_INLINE bool is_valid() const { return !m_is_error; }
    NODISCARD ALWAYS_INLINE bool is_error() const { return m_is_error; }
    NODISCARD ALWAYS_INLINE FileError error_code() const { return m_error_code; }
    NODISCARD ALWAYS_INLINE StringView error_message() const { return m_error_message; }

private:
    bool m_is_error;
    FileError m_error_code;
    StringView m_error_message;
};

enum class FileMode {
    Read,
    Write,
    Append,
};

using FileHandle = void*;
// FIXME: Find a more robust way to define the value of an invalid file handle. Because this
//        value depends on the underlying platform, we cannot use a generic constant. However,
//        by making this an external variable, there's no way to stop the program from modifying
//        the value at runtime!
extern FileHandle invalid_file_handle;

FileErrorOr<FileHandle> open_file(StringView const& filename, FileMode);
void close_file(FileHandle);

FileErrorOr<usize> read_from_file(FileHandle, void* dst_buffer, usize size_in_bytes);
FileErrorOr<void> write_to_file(FileHandle, void const* src_buffer, usize size_in_bytes);

FileErrorOr<u64> file_size(FileHandle);
FileErrorOr<u64> file_size(StringView const& filename);

FileErrorOr<bool> is_directory(StringView const& filepath);

} // namespace AND::Platform
