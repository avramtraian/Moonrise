/*
 * Copyright (c) 2024-2026. All rights reserved.
 * SPDX-License-Identifier: BSD-3-Clause.
 */

#include "AND/Defines.h"
#if AND_PLATFORM_OS_WINDOWS

#    include "AND/MathUtilities.h"
#    include "AND/NumericLimits.h"
#    include "AND/Platform.h"
#    include "AND/StringBuilder.h"
#    include "AND/Utf16String.h"

// clang-format off
#    define WIN32_LEAN_AND_MEAN
#    define NOMINMAX
#    include <Windows.h>
#    include <DbgHelp.h>
// clang-format on

namespace AND::Platform {

//=================================================================================================
// STANDARD I/O STREAMS.
//=================================================================================================

static HANDLE win32_get_standard_stream_handle(StdStream stream)
{
    switch (stream) {
    case StdStream::Output:
        return GetStdHandle(STD_OUTPUT_HANDLE);
    case StdStream::Error:
        return GetStdHandle(STD_ERROR_HANDLE);
    default:
        return INVALID_HANDLE_VALUE;
    }
}

static bool win32_std_stream_is_console(HANDLE handle)
{
    if (handle == INVALID_HANDLE_VALUE)
        return false;

    DWORD console_mode;
    BOOL result = GetConsoleMode(handle, &console_mode);
    return (result != 0);
}

static Result win32_write_file(HANDLE handle, ROBytes bytes, u64 byte_count)
{
    u64 bytes_written_so_far = 0;
    static constexpr u64 max_bytes_to_write = NumericLimits<DWORD>::max();
    while (bytes_written_so_far < byte_count) {
        DWORD bytes_to_write = static_cast<DWORD>(min(byte_count - bytes_written_so_far, max_bytes_to_write));
        DWORD bytes_written;
        if (!WriteFile(handle, bytes + bytes_written_so_far, bytes_to_write, &bytes_written, nullptr))
            return Result::UnknownError;
        if (bytes_written == 0)
            return Result::UnknownError;
        bytes_written_so_far += bytes_written;
    }

    return Result::Success;
}

static Result win32_write_console(HANDLE handle, wchar_t const* code_units, u64 code_unit_count)
{
    u64 code_units_written_so_far = 0;
    static constexpr u64 max_code_units_to_write = NumericLimits<DWORD>::max();

    while (code_units_written_so_far < code_unit_count) {
        DWORD code_units_to_write = static_cast<DWORD>(min(code_unit_count - code_units_written_so_far, max_code_units_to_write));
        DWORD code_units_written;
        if (!WriteConsoleW(handle, code_units + code_units_written_so_far, code_units_to_write, &code_units_written, nullptr))
            return Result::UnknownError;
        if (code_units_written == 0)
            return Result::UnknownError;
        code_units_written_so_far += code_units_written;
    }

    return Result::Success;
}

Result write_text_to_std_stream(StdStream stream, Utf8View contents)
{
    HANDLE stream_handle = win32_get_standard_stream_handle(stream);
    if (stream_handle == INVALID_HANDLE_VALUE)
        return Result::NoStdStream;

    if (!win32_std_stream_is_console(stream_handle)) {
        win32_write_file(stream_handle, contents.bytes(), contents.byte_count());
        return Result::Success;
    }

    Utf16String text = Utf16String::from_utf8(contents);
    wchar_t const* code_units = reinterpret_cast<wchar_t const*>(text.code_units());
    u64 code_unit_count = text.code_unit_count_without_null_terminator();

    return win32_write_console(stream_handle, code_units, code_unit_count);
}

Result write_binary_to_std_stream(StdStream stream, ROByteSpan buffer)
{
    HANDLE stream_handle = win32_get_standard_stream_handle(stream);
    if (stream_handle == INVALID_HANDLE_VALUE)
        return Result::NoStdStream;
    return win32_write_file(stream_handle, buffer.bytes(), buffer.count());
}

Result flush_std_stream(StdStream stream)
{
    HANDLE stream_handle = win32_get_standard_stream_handle(stream);
    if (stream_handle == INVALID_HANDLE_VALUE)
        return Result::NoStdStream;

    if (!win32_std_stream_is_console(stream_handle)) {
        if (FlushFileBuffers(stream_handle) == 0)
            return Result::UnknownError;
    }

    return Result::Success;
}

static WORD win32_get_console_color_flag(ConsoleColor foreground, ConsoleColor background)
{
    WORD foreground_flag = 0;
    // clang-format off
    switch (foreground) {
    case ConsoleColor::Black:           foreground_flag = 0;                                                                           break;
    case ConsoleColor::Red:             foreground_flag = FOREGROUND_RED;                                                              break;
    case ConsoleColor::Green:           foreground_flag = FOREGROUND_GREEN;                                                            break;
    case ConsoleColor::Blue:            foreground_flag = FOREGROUND_BLUE;                                                             break;
    case ConsoleColor::Yellow:          foreground_flag = FOREGROUND_RED | FOREGROUND_GREEN;                                           break;
    case ConsoleColor::Cyan:            foreground_flag = FOREGROUND_GREEN | FOREGROUND_BLUE;                                          break;
    case ConsoleColor::Magenta:         foreground_flag = FOREGROUND_BLUE | FOREGROUND_RED;                                            break;
    case ConsoleColor::LightGray:       foreground_flag = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;                         break;
    case ConsoleColor::Gray:            foreground_flag = FOREGROUND_INTENSITY;                                                        break;
    case ConsoleColor::LightRed:        foreground_flag = FOREGROUND_INTENSITY | FOREGROUND_RED;                                       break;
    case ConsoleColor::LightGreen:      foreground_flag = FOREGROUND_INTENSITY | FOREGROUND_GREEN;                                     break;
    case ConsoleColor::LightBlue:       foreground_flag = FOREGROUND_INTENSITY | FOREGROUND_BLUE;                                      break;
    case ConsoleColor::LightYellow:     foreground_flag = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN;                    break;
    case ConsoleColor::LightCyan:       foreground_flag = FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE;                   break;
    case ConsoleColor::LightMagenta:    foreground_flag = FOREGROUND_INTENSITY | FOREGROUND_BLUE | FOREGROUND_RED;                     break;
    case ConsoleColor::White:           foreground_flag = FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;  break;
    }
    // clang-format on

    WORD background_flag = 0;
    // clang-format off
    switch (background) {
    case ConsoleColor::Black:           background_flag = 0;                                                                           break;
    case ConsoleColor::Red:             background_flag = BACKGROUND_RED;                                                              break;
    case ConsoleColor::Green:           background_flag = BACKGROUND_GREEN;                                                            break;
    case ConsoleColor::Blue:            background_flag = BACKGROUND_BLUE;                                                             break;
    case ConsoleColor::Yellow:          background_flag = BACKGROUND_RED | BACKGROUND_GREEN;                                           break;
    case ConsoleColor::Cyan:            background_flag = BACKGROUND_GREEN | BACKGROUND_BLUE;                                          break;
    case ConsoleColor::Magenta:         background_flag = BACKGROUND_BLUE | BACKGROUND_RED;                                            break;
    case ConsoleColor::LightGray:       background_flag = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;                         break;
    case ConsoleColor::Gray:            background_flag = BACKGROUND_INTENSITY;                                                        break;
    case ConsoleColor::LightRed:        background_flag = BACKGROUND_INTENSITY | BACKGROUND_RED;                                       break;
    case ConsoleColor::LightGreen:      background_flag = BACKGROUND_INTENSITY | BACKGROUND_GREEN;                                     break;
    case ConsoleColor::LightBlue:       background_flag = BACKGROUND_INTENSITY | BACKGROUND_BLUE;                                      break;
    case ConsoleColor::LightYellow:     background_flag = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN;                    break;
    case ConsoleColor::LightCyan:       background_flag = BACKGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_BLUE;                   break;
    case ConsoleColor::LightMagenta:    background_flag = BACKGROUND_INTENSITY | BACKGROUND_BLUE | BACKGROUND_RED;                     break;
    case ConsoleColor::White:           background_flag = BACKGROUND_INTENSITY | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE;  break;
    }
    // clang-format on

    return foreground_flag | background_flag;
}

Result set_std_stream_colors(StdStream stream, Optional<ConsoleColor> foreground, Optional<ConsoleColor> background)
{
    HANDLE stream_handle = win32_get_standard_stream_handle(stream);
    if (stream_handle == INVALID_HANDLE_VALUE)
        return Result::NoStdStream;
    if (!win32_std_stream_is_console(stream_handle))
        return Result::StdStreamIsNotConsole;

    WORD color_flag = win32_get_console_color_flag(foreground.value_or(ConsoleColor::LightGray), background.value_or(ConsoleColor::Black));
    if (!SetConsoleTextAttribute(stream_handle, color_flag))
        return Result::UnknownError;

    return Result::Success;
}

//=================================================================================================
// DEBUG UTILITIES.
//=================================================================================================

Vector<CallStackFrame> get_call_stack(u32 frames_to_skip)
{
    void* backtrace[1024] = {};
    u32 frame_count = RtlCaptureStackBackTrace(frames_to_skip, ARRAY_COUNT(backtrace), backtrace, nullptr);

    HANDLE current_process = GetCurrentProcess();
    SymInitialize(current_process, nullptr, true);
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS | SYMOPT_LOAD_LINES);

    Vector<CallStackFrame> call_stack;
    call_stack.ensure_capacity(frame_count);

    for (u32 frame_index = 0; frame_index < frame_count; ++frame_index) {
        uintptr return_address = reinterpret_cast<uintptr>(backtrace[frame_index]);
        IMAGEHLP_MODULE64 module_info = {};
        module_info.SizeOfStruct = sizeof(IMAGEHLP_MODULE64);
        bool has_module_info = SymGetModuleInfo64(current_process, return_address, &module_info);

        static constexpr usize max_symbol_name_size = 1024;
        alignas(SYMBOL_INFO) RWByte symbol_info_buffer[sizeof(SYMBOL_INFO) + max_symbol_name_size] = {};
        SYMBOL_INFO* symbol_info = reinterpret_cast<SYMBOL_INFO*>(symbol_info_buffer);
        symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol_info->MaxNameLen = max_symbol_name_size;
        bool has_symbol_info = SymFromAddr(current_process, return_address, nullptr, symbol_info);

        IMAGEHLP_LINE64 line_info = {};
        line_info.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        DWORD displacement = 0;
        bool has_line_info = SymGetLineFromAddr64(current_process, return_address - 1, &displacement, &line_info);

        auto& frame = call_stack.emplace_back();
        frame.return_address = return_address;
        if (has_module_info)
            frame.module = Utf8View::from_null_terminated(module_info.ModuleName);
        if (has_symbol_info)
            frame.function = Utf8View::from_null_terminated(symbol_info->Name);
        if (has_line_info) {
            frame.file = Utf8View::from_null_terminated(line_info.FileName);
            frame.line = line_info.LineNumber;
        }
    }

    SymCleanup(current_process);
    return call_stack;
}

//=================================================================================================
// TIMING UTILITIES.
//=================================================================================================

// static u64 win32_get_performance_counter()
// {
//     LARGE_INTEGER counter;
//     if (!QueryPerformanceCounter(&counter))
//         PANIC("Failed to query 'QueryPerformanceCounter'!");
//     return counter.QuadPart;
// }

static u64 win32_get_performance_frequency()
{
    static u64 s_performance_frequency = 0;
    if (s_performance_frequency == 0) {
        LARGE_INTEGER frequency;
        if (!QueryPerformanceFrequency(&frequency))
            PANIC("Failed to query 'QueryPerformanceFrequency'!");
        s_performance_frequency = frequency.QuadPart;
    }
    return s_performance_frequency;
}

TimeDuration TimeDuration::from_milliseconds(f32 milliseconds)
{
    f32 duration_in_seconds = milliseconds / 1000.0F;
    return TimeDuration::from_seconds(duration_in_seconds);
}

TimeDuration TimeDuration::from_seconds(f32 seconds)
{
    u64 ticks = static_cast<u64>(seconds * win32_get_performance_frequency());
    return TimeDuration { ticks };
}

f32 TimeDuration::milliseconds() const
{
    f32 duration_in_seconds = seconds();
    return duration_in_seconds * 1000.0F;
}

f32 TimeDuration::seconds() const
{
    f64 ticks = static_cast<f64>(m_ticks);
    f64 ticks_per_second = static_cast<f64>(win32_get_performance_frequency());
    return static_cast<f32>(ticks / ticks_per_second);
}

void sleep(TimeDuration duration)
{
    DWORD milliseconds = static_cast<DWORD>(duration.milliseconds());
    Sleep(milliseconds);
}

//=================================================================================================
// FILE.
//=================================================================================================

FileHandle invalid_file_handle = INVALID_HANDLE_VALUE;

FileErrorOr<FileHandle> open_file(Utf8View const& filename, FileMode file_mode)
{
    DWORD desired_access = 0;
    DWORD share_mode = 0;
    DWORD creation_disposition = 0;

    // Find what flags should we pass to 'CreateFile'.
    if (file_mode == FileMode::Read) {
        desired_access = GENERIC_READ;
        share_mode = FILE_SHARE_READ;
        creation_disposition = OPEN_EXISTING;
    } else if (file_mode == FileMode::Write) {
        desired_access = GENERIC_WRITE;
        creation_disposition = CREATE_ALWAYS;
    } else if (file_mode == FileMode::Append) {
        desired_access = GENERIC_WRITE;
        creation_disposition = OPEN_ALWAYS;
    } else {
        ASSERT_NOT_REACHED;
    }

    // Create the file handle.
    auto utf16_filename = Utf16String::from_utf8(filename);
    HANDLE handle = CreateFileW(reinterpret_cast<LPCWSTR>(utf16_filename.code_units()), desired_access, share_mode, nullptr, creation_disposition, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (handle == INVALID_HANDLE_VALUE) {
        switch (GetLastError()) {
        case ERROR_FILE_NOT_FOUND:
            return { FileError::FileNotFound, VIEW("The provided file does not exist on disk!") };
        case ERROR_PATH_NOT_FOUND:
            return { FileError::FileNotFound, VIEW("The provided file path does not exist on disk!") };
        case ERROR_ACCESS_DENIED:
            return { FileError::PermissionDenied, VIEW("The process does not have permission to open the file!") };
        case ERROR_SHARING_VIOLATION:
            return { FileError::SharingViolation, VIEW("The file is already open in another process!") };
        default:
            return { FileError::Unknown, VIEW("Failed to open the file!") };
        }
    }

    // When appending, set the cursor to the end of the file.
    if (file_mode == FileMode::Append) {
        LARGE_INTEGER pointer_offset;
        pointer_offset.QuadPart = 0;
        if (!SetFilePointerEx(handle, pointer_offset, nullptr, FILE_END)) {
            switch (GetLastError()) {
            case ERROR_INVALID_HANDLE:
                return { FileError::InvalidHandle, VIEW("Invalid or closed file handle provided!") };
            default:
                return { FileError::Unknown, VIEW("Failed to set the file pointer!") };
            }
        }
    }

    return FileHandle { handle };
}

void close_file(FileHandle file_handle)
{
    if (file_handle == INVALID_HANDLE_VALUE)
        return;
    CloseHandle(file_handle);
}

FileErrorOr<usize> read_from_file(FileHandle file_handle, void* dst_buffer, usize size_in_bytes)
{
    if (file_handle == INVALID_HANDLE_VALUE)
        return { FileError::InvalidHandle, VIEW("Invalid file handle provided!") };

    // FIXME: Some very similar logic also happens in 'win32_write_to_file', which is used to
    //        write to the standard output/error streams. We should probably centralize them!

    u64 bytes_read_so_far = 0;
    u64 max_bytes_to_read = NumericLimits<DWORD>::max();
    WOBytes dst_bytes = static_cast<WOBytes>(dst_buffer);

    while (bytes_read_so_far < size_in_bytes) {
        DWORD bytes_to_read = static_cast<DWORD>(min(size_in_bytes - bytes_read_so_far, max_bytes_to_read));
        DWORD bytes_read;
        if (!ReadFile(file_handle, dst_bytes + bytes_read_so_far, bytes_to_read, &bytes_read, nullptr))
            return { FileError::Unknown, VIEW("Failed to read from the file!") };
        if (bytes_read == 0)
            break;
        bytes_read_so_far += bytes_read;
    }

    return bytes_read_so_far;
}

FileErrorOr<void> write_to_file(FileHandle file_handle, void const* src_buffer, usize size_in_bytes)
{
    if (file_handle == INVALID_HANDLE_VALUE)
        return { FileError::InvalidHandle, VIEW("Invalid file handle provided!") };

    // FIXME: Some very similar logic also happens in 'win32_write_to_file', which is used to
    //        write to the standard output/error streams. We should probably centralize them!

    u64 bytes_written_so_far = 0;
    u64 max_bytes_to_write = NumericLimits<DWORD>::max();
    ROBytes src_bytes = static_cast<ROBytes>(src_buffer);

    while (bytes_written_so_far < size_in_bytes) {
        DWORD bytes_to_write = static_cast<DWORD>(min(size_in_bytes - bytes_written_so_far, max_bytes_to_write));
        DWORD bytes_written;
        if (!WriteFile(file_handle, src_bytes + bytes_written_so_far, bytes_to_write, &bytes_written, nullptr))
            return { FileError::Unknown, VIEW("Failed to write to the file!") };
        if (bytes_written == 0)
            return { FileError::Unknown, VIEW("Failed to write to the file!") };
        bytes_written_so_far += bytes_written;
    }

    return {};
}

FileErrorOr<u64> file_size(FileHandle file_handle)
{
    if (file_handle == INVALID_HANDLE_VALUE)
        return { FileError::InvalidHandle, VIEW("Invalid file handle provided!") };

    LARGE_INTEGER file_size;
    if (!GetFileSizeEx(file_handle, &file_size)) {
        switch (GetLastError()) {
        case ERROR_INVALID_HANDLE:
            return { FileError::InvalidHandle, VIEW("Invalid or closed file handle provided!") };
        default:
            return { FileError::Unknown, VIEW("Failed to get the file size!") };
        }
    }

    return file_size.QuadPart;
}

static FileErrorOr<WIN32_FILE_ATTRIBUTE_DATA> win32_get_file_attributes(Utf8View const& filename)
{
    WIN32_FILE_ATTRIBUTE_DATA file_attributes;
    auto utf16_filename = Utf16String::from_utf8(filename);
    if (!GetFileAttributesExW(reinterpret_cast<LPCWSTR>(utf16_filename.code_units()), GetFileExInfoStandard, &file_attributes)) {
        switch (GetLastError()) {
        case ERROR_FILE_NOT_FOUND:
            return { FileError::FileNotFound, VIEW("The provided file does not exist on disk!") };
        case ERROR_PATH_NOT_FOUND:
            return { FileError::FileNotFound, VIEW("The provided file path does not exist on disk!") };
        case ERROR_ACCESS_DENIED:
            return { FileError::PermissionDenied, VIEW("The process does not have permission to read the file attributes!") };
        case ERROR_SHARING_VIOLATION:
            return { FileError::SharingViolation, VIEW("File is currently in use by another process!") };
        default:
            return { FileError::Unknown, VIEW("Failed to get the file attributes!") };
        }
    }

    return file_attributes;
}

FileErrorOr<u64> file_size(Utf8View const& filename)
{
    auto attributes_or_error = win32_get_file_attributes(filename);
    if (attributes_or_error.is_error())
        return { attributes_or_error.error_code(), attributes_or_error.error_message() };
    auto file_attributes = attributes_or_error.value();

    if (file_attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        return { FileError::IsDirectory, VIEW("File path represents a directory and not a file!") };

    u64 file_size_low = file_attributes.nFileSizeLow;
    u64 file_size_high = file_attributes.nFileSizeHigh;
    return (file_size_high << 32) | file_size_low;
}

FileErrorOr<bool> is_directory(Utf8View const& filepath)
{
    auto attributes_or_error = win32_get_file_attributes(filepath);
    if (attributes_or_error.is_error())
        return { attributes_or_error.error_code(), attributes_or_error.error_message() };
    auto file_attributes = attributes_or_error.value();

    return file_attributes.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
}

} // namespace AND::Platform

#endif
